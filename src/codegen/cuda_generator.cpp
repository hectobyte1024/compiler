#include "codegen/cuda_generator.h"

namespace sdl {

CUDAGenerator::CUDAGenerator() {
}

// Stub implementations for all visitor methods
void CUDAGenerator::visit(Type& node) {
    write(getTypeString(node));
}

void CUDAGenerator::visit(IdentifierExpression& node) {
    write(node.name);
}

void CUDAGenerator::visit(LiteralExpression& node) {
    write(node.value);
}

void CUDAGenerator::visit(BinaryExpression& node) {
    node.left->accept(*this);
    write(" " + getBinaryOperatorString(node.op) + " ");
    node.right->accept(*this);
}

void CUDAGenerator::visit(UnaryExpression& node) {
    write(getUnaryOperatorString(node.op));
    node.operand->accept(*this);
}

void CUDAGenerator::visit(FunctionCallExpression& node) {
    write(node.functionName + "(");
    for (size_t i = 0; i < node.arguments.size(); ++i) {
        if (i > 0) write(", ");
        node.arguments[i]->accept(*this);
    }
    write(")");
}

void CUDAGenerator::visit(MemberAccessExpression& node) {
    node.object->accept(*this);
    write("." + node.member);
}

void CUDAGenerator::visit(ExpressionStatement& node) {
    indent();
    node.expression->accept(*this);
    write(";\n");
}

void CUDAGenerator::visit(AssignmentStatement& node) {
    indent();
    node.target->accept(*this);
    write(" = ");
    node.value->accept(*this);
    write(";\n");
}

void CUDAGenerator::visit(VariableDeclaration& node) {
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

void CUDAGenerator::visit(FunctionDeclaration& node) {
    // CUDA device function
    write("__device__ ");
    
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

void CUDAGenerator::visit(ShaderDeclaration& node) {
    writeLine("// CUDA Kernel: " + node.name);
    
    // Generate kernel signature based on shader type
    std::string kernelSig = generateKernelSignature(node);
    writeLine(kernelSig + " {");
    increaseIndent();
    
    // Add thread indexing for compute shaders
    if (node.shaderType == ShaderDeclaration::ShaderType::COMPUTE) {
        writeLine("// Thread indexing");
        writeLine("int idx = blockIdx.x * blockDim.x + threadIdx.x;");
        writeLine("int idy = blockIdx.y * blockDim.y + threadIdx.y;");
        writeLine("");
    }
    
    // Process shader body
    inKernel_ = true;
    for (auto& stmt : node.body) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
    inKernel_ = false;
    
    decreaseIndent();
    writeLine("}");
    writeLine("");
}

void CUDAGenerator::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
}

void CUDAGenerator::visit(IfStatement& node) {
    // Stub implementation
}

void CUDAGenerator::visit(ForStatement& node) {
    write("for (");
    if (node.initialization) {
        node.initialization->accept(*this);
    }
    write("; ");
    if (node.condition) {
        node.condition->accept(*this);
    }
    write("; ");
    if (node.update) {
        node.update->accept(*this);
    }
    writeLine(") {");
    increaseIndent();
    if (node.body) {
        node.body->accept(*this);
    }
    decreaseIndent();
    writeLine("}");
}

void CUDAGenerator::visit(WhileStatement& node) {
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

void CUDAGenerator::visit(ReturnStatement& node) {
    // Stub implementation
}

void CUDAGenerator::visit(Program& node) {
    for (auto& decl : node.declarations) {
        if (decl) {
            decl->accept(*this);
        }
    }
}

std::string CUDAGenerator::getTypeString(const Type& type) {
    switch (type.kind) {
        case Type::Kind::VOID: return "void";
        case Type::Kind::BOOL: return "bool";
        case Type::Kind::INT: return "int";
        case Type::Kind::FLOAT: return "float";
        case Type::Kind::VEC2: return "float2";
        case Type::Kind::VEC3: return "float3";
        case Type::Kind::VEC4: return "float4";
        case Type::Kind::MAT2: return "float2x2";
        case Type::Kind::MAT3: return "float3x3";
        case Type::Kind::MAT4: return "float4x4";
        case Type::Kind::SAMPLER2D: return "cudaTextureObject_t";
        case Type::Kind::SAMPLER3D: return "cudaTextureObject_t";
        case Type::Kind::SAMPLERCUBE: return "cudaTextureObject_t";
        default: return "unknown";
    }
}

std::string CUDAGenerator::getQualifierString(VariableDeclaration::Qualifier qualifier) {
    switch (qualifier) {
        case VariableDeclaration::Qualifier::IN: return "";
        case VariableDeclaration::Qualifier::OUT: return "";
        case VariableDeclaration::Qualifier::UNIFORM: return "__constant__";
        case VariableDeclaration::Qualifier::CONST: return "const";
        default: return "";
    }
}

std::string CUDAGenerator::getBinaryOperatorString(BinaryExpression::Operator op) {
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

std::string CUDAGenerator::getUnaryOperatorString(UnaryExpression::Operator op) {
    switch (op) {
        case UnaryExpression::Operator::MINUS: return "-";
        case UnaryExpression::Operator::LOGICAL_NOT: return "!";
        default: return "?";
    }
}

std::string CUDAGenerator::getFunctionCallString(const std::string& name, 
                                                const std::vector<std::string>& args) {
    std::string result = getCUDABuiltinFunction(name) + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) result += ", ";
        result += args[i];
    }
    result += ")";
    return result;
}

void CUDAGenerator::generatePreamble() {
    generateCUDAIncludes();
}

void CUDAGenerator::generatePostamble() {
    // Nothing needed for basic CUDA
}

void CUDAGenerator::generateCUDAIncludes() {
    writeLine("#include <cuda_runtime.h>");
    writeLine("#include <device_launch_parameters.h>");
    writeLine("");
}

std::string CUDAGenerator::getCUDABuiltinFunction(const std::string& name) {
    // Map GLSL functions to CUDA equivalents
    if (name == "texture") return "tex2D";
    if (name == "normalize") return "normalize";
    if (name == "dot") return "dot";
    if (name == "cross") return "cross";
    if (name == "length") return "length";
    if (name == "distance") return "distance";
    
    return name; // Return as-is if no mapping found
}

std::string CUDAGenerator::mapGLSLTypeToCUDA(const std::string& glslType) {
    if (glslType == "vec2") return "float2";
    if (glslType == "vec3") return "float3";
    if (glslType == "vec4") return "float4";
    if (glslType == "mat2") return "float2x2";
    if (glslType == "mat3") return "float3x3";
    if (glslType == "mat4") return "float4x4";
    if (glslType == "sampler2D") return "cudaTextureObject_t";
    
    return glslType;
}

std::string CUDAGenerator::generateKernelSignature(const ShaderDeclaration& shader) {
    std::string signature = "__global__ void " + shader.name + "_kernel(";
    
    // Add parameters based on shader type
    switch (shader.shaderType) {
        case ShaderDeclaration::ShaderType::VERTEX:
            signature += "float* vertices, float* output, int numVertices";
            break;
        case ShaderDeclaration::ShaderType::FRAGMENT:
            signature += "float* pixels, int width, int height";
            break;
        case ShaderDeclaration::ShaderType::COMPUTE:
            signature += "float* input, float* output, int width, int height";
            break;
    }
    
    signature += ")";
    return signature;
}

} // namespace sdl
