#pragma once

#include "parser/ast.h"

namespace sdl {

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Types
    virtual void visit(Type& node) = 0;
    
    // Expressions
    virtual void visit(IdentifierExpression& node) = 0;
    virtual void visit(LiteralExpression& node) = 0;
    virtual void visit(BinaryExpression& node) = 0;
    virtual void visit(UnaryExpression& node) = 0;
    virtual void visit(FunctionCallExpression& node) = 0;
    virtual void visit(MemberAccessExpression& node) = 0;
    
    // Statements
    virtual void visit(ExpressionStatement& node) = 0;
    virtual void visit(AssignmentStatement& node) = 0;
    virtual void visit(VariableDeclaration& node) = 0;
    virtual void visit(FunctionDeclaration& node) = 0;
    virtual void visit(ShaderDeclaration& node) = 0;
    virtual void visit(BlockStatement& node) = 0;
    virtual void visit(IfStatement& node) = 0;
    virtual void visit(ForStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    
    // Program
    virtual void visit(Program& node) = 0;
};

} // namespace sdl
