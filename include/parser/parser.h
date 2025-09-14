#pragma once

#include "lexer/lexer.h"
#include "parser/ast.h"
#include <memory>

namespace sdl {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    
    std::unique_ptr<Program> parseProgram();
    
private:
    std::vector<Token> tokens_;
    size_t current_;
    
    // Utility methods
    Token& currentToken();
    Token& peekToken(size_t offset = 1);
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
    void advance();
    
    // Parsing methods
    StatementPtr parseDeclaration();
    StatementPtr parseShaderDeclaration();
    StatementPtr parseFunctionDeclaration();
    StatementPtr parseVariableDeclaration();
    StatementPtr parseStatement();
    StatementPtr parseBlockStatement();
    StatementPtr parseExpressionStatement();
    StatementPtr parseAssignmentStatement();
    StatementPtr parseIfStatement();
    StatementPtr parseForStatement();
    StatementPtr parseWhileStatement();
    StatementPtr parseReturnStatement();
    
    ExpressionPtr parseExpression();
    ExpressionPtr parseAssignment();
    ExpressionPtr parseLogicalOr();
    ExpressionPtr parseLogicalAnd();
    ExpressionPtr parseEquality();
    ExpressionPtr parseComparison();
    ExpressionPtr parseAddition();
    ExpressionPtr parseMultiplication();
    ExpressionPtr parseUnary();
    ExpressionPtr parsePrimary();
    ExpressionPtr parsePostfix();
    ExpressionPtr parseFunctionCall(const std::string& name);
    
    TypePtr parseType();
    VariableDeclaration::Qualifier parseQualifier();
    ShaderDeclaration::ShaderType parseShaderType();
    
    void synchronize();
    void reportError(const std::string& message);
};

} // namespace sdl
