#include "parser/ast.h"
#include "parser/ast_visitor.h"

namespace sdl {

// Type implementations
void Type::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// Expression implementations
void IdentifierExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void LiteralExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void BinaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionCallExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void MemberAccessExpression::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// Statement implementations
void ExpressionStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void AssignmentStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void VariableDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ShaderDeclaration::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void BlockStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void IfStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ForStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void WhileStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ReturnStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void Program::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace sdl
