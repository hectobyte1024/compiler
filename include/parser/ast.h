#pragma once

#include <memory>
#include <string>
#include <vector>

namespace sdl {

// Forward declarations
class ASTNode;
class Expression;
class Statement;
class Type;

using ASTNodePtr = std::unique_ptr<ASTNode>;
using ExpressionPtr = std::unique_ptr<Expression>;
using StatementPtr = std::unique_ptr<Statement>;
using TypePtr = std::unique_ptr<Type>;

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(class ASTVisitor& visitor) = 0;
};

// Types
class Type : public ASTNode {
public:
    enum class Kind {
        VOID, BOOL, INT, FLOAT,
        VEC2, VEC3, VEC4,
        MAT2, MAT3, MAT4,
        SAMPLER2D, SAMPLER3D, SAMPLERCUBE,
        STRUCT, ARRAY
    };
    
    Kind kind;
    std::string name; // For struct types
    int arraySize = -1; // For array types, -1 means not an array
    
    explicit Type(Kind k) : kind(k) {}
    Type(Kind k, const std::string& n) : kind(k), name(n) {}
    
    void accept(ASTVisitor& visitor) override;
};

// Expressions
class Expression : public ASTNode {
public:
    TypePtr resultType;
};

class IdentifierExpression : public Expression {
public:
    std::string name;
    
    explicit IdentifierExpression(const std::string& n) : name(n) {}
    void accept(ASTVisitor& visitor) override;
};

class LiteralExpression : public Expression {
public:
    enum class LiteralType { INT, FLOAT, BOOL, STRING };
    LiteralType literalType;
    std::string value;
    
    LiteralExpression(LiteralType t, const std::string& v) 
        : literalType(t), value(v) {}
    void accept(ASTVisitor& visitor) override;
};

class BinaryExpression : public Expression {
public:
    enum class Operator {
        ASSIGN,
        ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO,
        EQUAL, NOT_EQUAL, LESS_THAN, LESS_EQUAL,
        GREATER_THAN, GREATER_EQUAL,
        LOGICAL_AND, LOGICAL_OR
    };
    
    ExpressionPtr left;
    Operator op;
    ExpressionPtr right;
    
    BinaryExpression(ExpressionPtr l, Operator o, ExpressionPtr r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    void accept(ASTVisitor& visitor) override;
};

class UnaryExpression : public Expression {
public:
    enum class Operator { MINUS, LOGICAL_NOT };
    
    Operator op;
    ExpressionPtr operand;
    
    UnaryExpression(Operator o, ExpressionPtr expr)
        : op(o), operand(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

class FunctionCallExpression : public Expression {
public:
    std::string functionName;
    std::vector<ExpressionPtr> arguments;
    
    explicit FunctionCallExpression(const std::string& name) 
        : functionName(name) {}
    void accept(ASTVisitor& visitor) override;
};

class MemberAccessExpression : public Expression {
public:
    ExpressionPtr object;
    std::string member;
    
    MemberAccessExpression(ExpressionPtr obj, const std::string& mem)
        : object(std::move(obj)), member(mem) {}
    void accept(ASTVisitor& visitor) override;
};

// Statements
class Statement : public ASTNode {
};

class ExpressionStatement : public Statement {
public:
    ExpressionPtr expression;
    
    explicit ExpressionStatement(ExpressionPtr expr)
        : expression(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

class AssignmentStatement : public Statement {
public:
    ExpressionPtr target;
    ExpressionPtr value;
    
    AssignmentStatement(ExpressionPtr t, ExpressionPtr v)
        : target(std::move(t)), value(std::move(v)) {}
    void accept(ASTVisitor& visitor) override;
};

class VariableDeclaration : public Statement {
public:
    enum class Qualifier { NONE, IN, OUT, UNIFORM, CONST };
    
    Qualifier qualifier;
    TypePtr type;
    std::string name;
    ExpressionPtr initializer;
    
    VariableDeclaration(Qualifier q, TypePtr t, const std::string& n)
        : qualifier(q), type(std::move(t)), name(n) {}
    void accept(ASTVisitor& visitor) override;
};

class FunctionDeclaration : public Statement {
public:
    std::string name;
    TypePtr returnType;
    std::vector<std::unique_ptr<VariableDeclaration>> parameters;
    std::vector<StatementPtr> body;
    
    FunctionDeclaration(const std::string& n, TypePtr ret)
        : name(n), returnType(std::move(ret)) {}
    void accept(ASTVisitor& visitor) override;
};

class ShaderDeclaration : public Statement {
public:
    enum class ShaderType { VERTEX, FRAGMENT, COMPUTE };
    
    std::string name;
    ShaderType shaderType;
    std::vector<StatementPtr> body;
    
    ShaderDeclaration(const std::string& n, ShaderType t)
        : name(n), shaderType(t) {}
    void accept(ASTVisitor& visitor) override;
};

class BlockStatement : public Statement {
public:
    std::vector<StatementPtr> statements;
    
    void accept(ASTVisitor& visitor) override;
};

class IfStatement : public Statement {
public:
    ExpressionPtr condition;
    StatementPtr thenStatement;
    StatementPtr elseStatement;
    
    IfStatement(ExpressionPtr cond, StatementPtr then, StatementPtr else_stmt = nullptr)
        : condition(std::move(cond)), thenStatement(std::move(then)), 
          elseStatement(std::move(else_stmt)) {}
    void accept(ASTVisitor& visitor) override;
};

class ForStatement : public Statement {
public:
    StatementPtr initialization;
    ExpressionPtr condition;
    StatementPtr update;
    StatementPtr body;
    
    ForStatement(StatementPtr init, ExpressionPtr cond, 
                 StatementPtr upd, StatementPtr bod)
        : initialization(std::move(init)), condition(std::move(cond)),
          update(std::move(upd)), body(std::move(bod)) {}
    void accept(ASTVisitor& visitor) override;
};

class WhileStatement : public Statement {
public:
    ExpressionPtr condition;
    StatementPtr body;
    
    WhileStatement(ExpressionPtr cond, StatementPtr bod)
        : condition(std::move(cond)), body(std::move(bod)) {}
    void accept(ASTVisitor& visitor) override;
};

class ReturnStatement : public Statement {
public:
    ExpressionPtr value;
    
    explicit ReturnStatement(ExpressionPtr val = nullptr)
        : value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override;
};

// Program (root node)
class Program : public ASTNode {
public:
    std::vector<StatementPtr> declarations;
    
    void accept(ASTVisitor& visitor) override;
};

} // namespace sdl
