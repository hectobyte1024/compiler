#include "codegen/glsl_generator.h"

namespace sdl {

GLSLGenerator::GLSLGenerator() {
}

// Stub implementations for all visitor methods
void GLSLGenerator::visit(Type& node) {
    write(getTypeString(node));
}

void GLSLGenerator::visit(IdentifierExpression& node) {
    write(node.name);
}

void GLSLGenerator::visit(LiteralExpression& node) {
    write(node.value);
}

void GLSLGenerator::visit(BinaryExpression& node) {
    write("(");
    node.left->accept(*this);
    write(" " + getBinaryOperatorString(node.op) + " ");
    node.right->accept(*this);
    write(")");
}

void GLSLGenerator::visit(UnaryExpression& node) {
    write(getUnaryOperatorString(node.op));
    node.operand->accept(*this);
}

void GLSLGenerator::visit(FunctionCallExpression& node) {
    write(node.functionName + "(");
    for (size_t i = 0; i < node.arguments.size(); ++i) {
        if (i > 0) write(", ");
        node.arguments[i]->accept(*this);
    }
    write(")");
}

void GLSLGenerator::visit(MemberAccessExpression& node) {
    if (node.object) {
        node.object->accept(*this);
    }
    write("." + node.member);
}

void GLSLGenerator::visit(ExpressionStatement& node) {
    indent();
    node.expression->accept(*this);
    write(";\n");
}

void GLSLGenerator::visit(AssignmentStatement& node) {
    indent();
    node.target->accept(*this);
    write(" = ");
    node.value->accept(*this);
    write(";\n");
}

void GLSLGenerator::visit(VariableDeclaration& node) {
    std::string qualifier = getQualifierString(node.qualifier);
    if (!qualifier.empty()) {
        write(qualifier + " ");
    }
    
    if (node.type) {
        node.type->accept(*this);
        write(" ");
    }
    
    write(node.name);
    
    if (node.initializer) {
        write(" = ");
        node.initializer->accept(*this);
    }
    
    writeLine(";");
}

void GLSLGenerator::visit(FunctionDeclaration& node) {
    if (node.returnType) {
        node.returnType->accept(*this);
    } else {
        write("void");
    }
    
    write(" " + node.name + "(");
    
    // Parameters
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        if (i > 0) write(", ");
        auto& param = node.parameters[i];
        
        std::string qualifier = getQualifierString(param->qualifier);
        if (!qualifier.empty()) {
            write(qualifier + " ");
        }
        
        if (param->type) {
            param->type->accept(*this);
        }
        write(" " + param->name);
    }
    
    writeLine(") {");
    increaseIndent();
    
    // Function body
    for (auto& stmt : node.body) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
    
    decreaseIndent();
    writeLine("}");
    writeLine("");
}

void GLSLGenerator::visit(ShaderDeclaration& node) {
    writeLine("// Shader: " + node.name);
    for (auto& stmt : node.body) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
}

void GLSLGenerator::visit(BlockStatement& node) {
    writeLine("{");
    increaseIndent();
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
    decreaseIndent();
    writeLine("}");
}

void GLSLGenerator::visit(IfStatement& node) {
    indent();
    write("if (");
    if (node.condition) {
        node.condition->accept(*this);
    }
    writeLine(") {");
    increaseIndent();
    if (node.thenStatement) {
        node.thenStatement->accept(*this);
    }
    decreaseIndent();
    indent();
    if (node.elseStatement) {
        writeLine("} else {");
        increaseIndent();
        node.elseStatement->accept(*this);
        decreaseIndent();
        indent();
    }
    writeLine("}");
}

void GLSLGenerator::visit(ForStatement& node) {
    indent();
    write("for (");
    if (node.initialization) {
        // For variable declarations in for-loop, don't add semicolon here
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(node.initialization.get())) {
            std::string qualifier = getQualifierString(varDecl->qualifier);
            if (!qualifier.empty()) {
                write(qualifier + " ");
            }
            
            if (varDecl->type) {
                varDecl->type->accept(*this);
                write(" ");
            }
            
            write(varDecl->name);
            
            if (varDecl->initializer) {
                write(" = ");
                varDecl->initializer->accept(*this);
            }
        } else {
            node.initialization->accept(*this);
        }
    }
    write("; ");
    if (node.condition) {
        node.condition->accept(*this);
    }
    write("; ");
    if (node.update) {
        // For expression statements in update clause, don't include semicolon
        if (auto exprStmt = dynamic_cast<ExpressionStatement*>(node.update.get())) {
            if (exprStmt->expression) {
                exprStmt->expression->accept(*this);
            }
        } else {
            node.update->accept(*this);
        }
    }
    writeLine(") {");
    increaseIndent();
    if (node.body) {
        node.body->accept(*this);
    }
    decreaseIndent();
    indent();
    writeLine("}");
}

void GLSLGenerator::visit(WhileStatement& node) {
    write("while (");
    if (node.condition) {
        node.condition->accept(*this);
    }
    writeLine(") {");
    increaseIndent();
    if (node.body) {
        node.body->accept(*this);
    }
    decreaseIndent();
    writeLine("}");
}

void GLSLGenerator::visit(ReturnStatement& node) {
    indent();
    write("return");
    if (node.value) {
        write(" ");
        node.value->accept(*this);
    }
    writeLine(";");
}

void GLSLGenerator::visit(Program& node) {
    for (auto& decl : node.declarations) {
        if (decl) {
            decl->accept(*this);
        }
    }
}

std::string GLSLGenerator::getTypeString(const Type& type) {
    switch (type.kind) {
        case Type::Kind::VOID: return "void";
        case Type::Kind::BOOL: return "bool";
        case Type::Kind::INT: return "int";
        case Type::Kind::FLOAT: return "float";
        case Type::Kind::VEC2: return "vec2";
        case Type::Kind::VEC3: return "vec3";
        case Type::Kind::VEC4: return "vec4";
        case Type::Kind::MAT2: return "mat2";
        case Type::Kind::MAT3: return "mat3";
        case Type::Kind::MAT4: return "mat4";
        case Type::Kind::SAMPLER2D: return "sampler2D";
        case Type::Kind::SAMPLER3D: return "sampler3D";
        case Type::Kind::SAMPLERCUBE: return "samplerCube";
        default: return "unknown";
    }
}

std::string GLSLGenerator::getQualifierString(VariableDeclaration::Qualifier qualifier) {
    switch (qualifier) {
        case VariableDeclaration::Qualifier::IN: return "in";
        case VariableDeclaration::Qualifier::OUT: return "out";
        case VariableDeclaration::Qualifier::UNIFORM: return "uniform";
        case VariableDeclaration::Qualifier::CONST: return "const";
        default: return "";
    }
}

std::string GLSLGenerator::getBinaryOperatorString(BinaryExpression::Operator op) {
    switch (op) {
        case BinaryExpression::Operator::ASSIGN: return "=";
        case BinaryExpression::Operator::ADD: return "+";
        case BinaryExpression::Operator::SUBTRACT: return "-";
        case BinaryExpression::Operator::MULTIPLY: return "*";
        case BinaryExpression::Operator::DIVIDE: return "/";
        case BinaryExpression::Operator::MODULO: return "%";
        case BinaryExpression::Operator::EQUAL: return "==";
        case BinaryExpression::Operator::NOT_EQUAL: return "!=";
        case BinaryExpression::Operator::LESS_THAN: return "<";
        case BinaryExpression::Operator::LESS_EQUAL: return "<=";
        case BinaryExpression::Operator::GREATER_THAN: return ">";
        case BinaryExpression::Operator::GREATER_EQUAL: return ">=";
        case BinaryExpression::Operator::LOGICAL_AND: return "&&";
        case BinaryExpression::Operator::LOGICAL_OR: return "||";
        default: return "?";
    }
}

std::string GLSLGenerator::getUnaryOperatorString(UnaryExpression::Operator op) {
    switch (op) {
        case UnaryExpression::Operator::MINUS: return "-";
        case UnaryExpression::Operator::LOGICAL_NOT: return "!";
        default: return "?";
    }
}

std::string GLSLGenerator::getFunctionCallString(const std::string& name, 
                                                const std::vector<std::string>& args) {
    std::string result = name + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) result += ", ";
        result += args[i];
    }
    result += ")";
    return result;
}

void GLSLGenerator::generatePreamble() {
    generateGLSLVersion();
}

void GLSLGenerator::generatePostamble() {
    // Nothing needed for basic GLSL
}

void GLSLGenerator::generateGLSLVersion() {
    writeLine("#version 330 core");
    writeLine("");
}

std::string GLSLGenerator::getGLSLBuiltinFunction(const std::string& name) {
    // Return GLSL built-in function names as-is
    return name;
}

} // namespace sdl
