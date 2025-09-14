#include "parser/parser.h"
#include <stdexcept>
#include <iostream>

namespace sdl {

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)), current_(0) {
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    
    while (!isAtEnd()) {
        try {
            auto decl = parseDeclaration();
            if (decl) {
                program->declarations.push_back(std::move(decl));
            }
        } catch (const std::exception& e) {
            reportError(e.what());
            synchronize();
        }
    }
    
    return program;
}

Token& Parser::currentToken() {
    if (isAtEnd()) {
        static Token eofToken(TokenType::END_OF_FILE, "", 0, 0);
        return eofToken;
    }
    return tokens_[current_];
}

Token& Parser::peekToken(size_t offset) {
    size_t pos = current_ + offset;
    if (pos >= tokens_.size()) {
        static Token eofToken(TokenType::END_OF_FILE, "", 0, 0);
        return eofToken;
    }
    return tokens_[pos];
}

bool Parser::isAtEnd() const {
    return current_ >= tokens_.size() || tokens_[current_].type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return tokens_[current_].type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        Token token = currentToken();
        advance();
        return token;
    }
    throw std::runtime_error(message);
}

void Parser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
}

StatementPtr Parser::parseDeclaration() {
    if (match(TokenType::SHADER)) {
        return parseShaderDeclaration();
    }
    
    // Try to parse as variable declaration first
    if (check(TokenType::IN) || check(TokenType::OUT) || check(TokenType::UNIFORM) || check(TokenType::CONST) ||
        check(TokenType::VOID) || check(TokenType::BOOL) || check(TokenType::INT) || check(TokenType::FLOAT) ||
        check(TokenType::VEC2) || check(TokenType::VEC3) || check(TokenType::VEC4) ||
        check(TokenType::MAT2) || check(TokenType::MAT3) || check(TokenType::MAT4) ||
        check(TokenType::SAMPLER2D) || check(TokenType::SAMPLER3D) || check(TokenType::SAMPLERCUBE)) {
        
        // Check for qualifiers
        VariableDeclaration::Qualifier qualifier = parseQualifier();
        
        // Parse type
        TypePtr type = parseType();
        
        // Check if this is a function declaration (has identifier followed by '(')
        if (check(TokenType::IDENTIFIER)) {
            Token nameToken = consume(TokenType::IDENTIFIER, "Expected identifier");
            
            if (check(TokenType::LEFT_PAREN)) {
                // Function declaration
                auto func = std::make_unique<FunctionDeclaration>(nameToken.value, std::move(type));
                
                consume(TokenType::LEFT_PAREN, "Expected '('");
                
                // Parse parameters
                if (!check(TokenType::RIGHT_PAREN)) {
                    do {
                        VariableDeclaration::Qualifier paramQualifier = parseQualifier();
                        TypePtr paramType = parseType();
                        Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
                        
                        auto param = std::make_unique<VariableDeclaration>(
                            paramQualifier, std::move(paramType), paramName.value);
                        func->parameters.push_back(std::move(param));
                    } while (match(TokenType::COMMA));
                }
                
                consume(TokenType::RIGHT_PAREN, "Expected ')'");
                
                if (match(TokenType::LEFT_BRACE)) {
                    // Function body
                    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        auto stmt = parseStatement();
                        if (stmt) {
                            func->body.push_back(std::move(stmt));
                        }
                    }
                    consume(TokenType::RIGHT_BRACE, "Expected '}'");
                } else {
                    consume(TokenType::SEMICOLON, "Expected ';' after function declaration");
                }
                
                return std::move(func);
            } else {
                // Variable declaration
                auto varDecl = std::make_unique<VariableDeclaration>(
                    qualifier, std::move(type), nameToken.value);
                
                if (match(TokenType::ASSIGN)) {
                    varDecl->initializer = parseExpression();
                }
                
                consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
                return std::move(varDecl);
            }
        }
        
        throw std::runtime_error("Expected identifier after type");
    }
    
    // If not a clear declaration, try parsing as a statement
    return parseStatement();
}

StatementPtr Parser::parseShaderDeclaration() {
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected shader name");
    consume(TokenType::COLON, "Expected ':' after shader name");
    
    ShaderDeclaration::ShaderType shaderType = parseShaderType();
    
    auto shader = std::make_unique<ShaderDeclaration>(nameToken.value, shaderType);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' to begin shader body");
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Parse declarations within the shader body
        if (check(TokenType::IN) || check(TokenType::OUT) || check(TokenType::UNIFORM) || check(TokenType::CONST)) {
            // Variable declaration with qualifier
            VariableDeclaration::Qualifier qualifier = parseQualifier();
            TypePtr type = parseType();
            Token varName = consume(TokenType::IDENTIFIER, "Expected variable name");
            
            auto varDecl = std::make_unique<VariableDeclaration>(qualifier, std::move(type), varName.value);
            
            if (match(TokenType::ASSIGN)) {
                varDecl->initializer = parseExpression();
            }
            
            consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
            shader->body.push_back(std::move(varDecl));
            
        } else if (check(TokenType::VOID) || check(TokenType::BOOL) || check(TokenType::INT) || 
                   check(TokenType::FLOAT) || check(TokenType::VEC2) || check(TokenType::VEC3) || 
                   check(TokenType::VEC4) || check(TokenType::MAT2) || check(TokenType::MAT3) || 
                   check(TokenType::MAT4) || check(TokenType::SAMPLER2D) || check(TokenType::SAMPLER3D) || 
                   check(TokenType::SAMPLERCUBE)) {
            
            // Function or variable declaration
            TypePtr returnType = parseType();
            Token name = consume(TokenType::IDENTIFIER, "Expected name");
            
            if (check(TokenType::LEFT_PAREN)) {
                // Function declaration
                auto func = std::make_unique<FunctionDeclaration>(name.value, std::move(returnType));
                
                consume(TokenType::LEFT_PAREN, "Expected '('");
                
                // Parse parameters
                if (!check(TokenType::RIGHT_PAREN)) {
                    do {
                        VariableDeclaration::Qualifier paramQualifier = parseQualifier();
                        TypePtr paramType = parseType();
                        Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
                        
                        auto param = std::make_unique<VariableDeclaration>(
                            paramQualifier, std::move(paramType), paramName.value);
                        func->parameters.push_back(std::move(param));
                    } while (match(TokenType::COMMA));
                }
                
                consume(TokenType::RIGHT_PAREN, "Expected ')'");
                
                if (match(TokenType::LEFT_BRACE)) {
                    // Function body - parse statements
                    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        auto stmt = parseStatement();
                        if (stmt) {
                            func->body.push_back(std::move(stmt));
                        }
                    }
                    consume(TokenType::RIGHT_BRACE, "Expected '}'");
                } else {
                    consume(TokenType::SEMICOLON, "Expected ';' after function declaration");
                }
                
                shader->body.push_back(std::move(func));
            } else {
                // Variable declaration without qualifier
                auto varDecl = std::make_unique<VariableDeclaration>(
                    VariableDeclaration::Qualifier::NONE, std::move(returnType), name.value);
                
                if (match(TokenType::ASSIGN)) {
                    varDecl->initializer = parseExpression();
                }
                
                consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
                shader->body.push_back(std::move(varDecl));
            }
        } else {
            throw std::runtime_error("Unexpected token in shader body at line " + 
                                    std::to_string(currentToken().line) + ", column " + 
                                    std::to_string(currentToken().column));
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' to end shader body");
    
    return std::move(shader);
}

StatementPtr Parser::parseFunctionDeclaration() {
    // Stub implementation
    return nullptr;
}

StatementPtr Parser::parseVariableDeclaration() {
    VariableDeclaration::Qualifier qualifier = parseQualifier();
    TypePtr type = parseType();
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    auto varDecl = std::make_unique<VariableDeclaration>(
        qualifier, std::move(type), nameToken.value);
    
    if (match(TokenType::ASSIGN)) {
        varDecl->initializer = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    return std::move(varDecl);
}

StatementPtr Parser::parseStatement() {
    if (match(TokenType::LEFT_BRACE)) {
        return parseBlockStatement();
    }
    
    if (match(TokenType::IF)) {
        return parseIfStatement();
    }
    
    if (match(TokenType::FOR)) {
        return parseForStatement();
    }
    
    if (match(TokenType::WHILE)) {
        return parseWhileStatement();
    }
    
    if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    
    // Check for variable declarations (type keywords followed by identifier)
    if (check(TokenType::VOID) || check(TokenType::BOOL) || check(TokenType::INT) || check(TokenType::FLOAT) ||
        check(TokenType::VEC2) || check(TokenType::VEC3) || check(TokenType::VEC4) ||
        check(TokenType::MAT2) || check(TokenType::MAT3) || check(TokenType::MAT4) ||
        check(TokenType::SAMPLER2D) || check(TokenType::SAMPLER3D) || check(TokenType::SAMPLERCUBE)) {
        
        // Look ahead to see if this is actually a variable declaration (type followed by identifier)
        // vs a function call (type followed by parenthesis)
        size_t savePos = current_;
        advance(); // Skip the type token
        
        bool isVariableDeclaration = check(TokenType::IDENTIFIER) && (peekToken().type != TokenType::LEFT_PAREN);
        current_ = savePos; // Restore position
        
        if (isVariableDeclaration) {
            return parseVariableDeclaration();
        }
        // Otherwise, fall through to parse as expression statement
    }
    
    // Check for assignment vs expression statement
    if (check(TokenType::IDENTIFIER)) {
        // Look ahead to see if this is an assignment
        size_t savePos = current_;
        advance(); // Consume the identifier
        
        if (check(TokenType::ASSIGN)) {
            // This is an assignment
            current_ = savePos; // Restore position
            Token identToken = consume(TokenType::IDENTIFIER, "Expected identifier");
            std::string identName = identToken.value;
            
            consume(TokenType::ASSIGN, "Expected '='");
            ExpressionPtr value = parseExpression();
            consume(TokenType::SEMICOLON, "Expected ';' after assignment");
            
            auto leftExpr = std::make_unique<IdentifierExpression>(identName);
            return std::make_unique<AssignmentStatement>(std::move(leftExpr), std::move(value));
        } else {
            // This is an expression statement
            current_ = savePos; // Restore position
            ExpressionPtr expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expected ';' after expression");
            return std::make_unique<ExpressionStatement>(std::move(expr));
        }
    }
    
    // All other statements are expression statements
    ExpressionPtr expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

StatementPtr Parser::parseBlockStatement() {
    auto block = std::make_unique<BlockStatement>();
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return std::move(block);
}

StatementPtr Parser::parseExpressionStatement() {
    // Stub implementation
    return nullptr;
}

StatementPtr Parser::parseAssignmentStatement() {
    // Stub implementation
    return nullptr;
}

StatementPtr Parser::parseIfStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    ExpressionPtr condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition");
    
    StatementPtr thenStmt = parseStatement();
    StatementPtr elseStmt = nullptr;
    
    if (match(TokenType::ELSE)) {
        elseStmt = parseStatement();
    }
    
    return std::make_unique<IfStatement>(
        std::move(condition), std::move(thenStmt), std::move(elseStmt));
}

StatementPtr Parser::parseForStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'");
    
    // Initialization - can be variable declaration or assignment/expression
    StatementPtr init = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        // Check if this is a variable declaration (type keyword followed by identifier)
        if (check(TokenType::INT) || check(TokenType::FLOAT) || check(TokenType::BOOL) ||
            check(TokenType::VEC2) || check(TokenType::VEC3) || check(TokenType::VEC4) ||
            check(TokenType::MAT2) || check(TokenType::MAT3) || check(TokenType::MAT4)) {
            
            // Variable declaration
            TypePtr type = parseType();
            Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name");
            
            auto varDecl = std::make_unique<VariableDeclaration>(
                VariableDeclaration::Qualifier::NONE, std::move(type), nameToken.value);
            
            if (match(TokenType::ASSIGN)) {
                varDecl->initializer = parseExpression();
            }
            
            init = std::move(varDecl);
        } else {
            // Assignment or expression statement
            init = parseStatement();
            // Don't consume semicolon here since parseStatement() already does
            current_--; // Back up one since parseStatement consumed the semicolon
        }
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for init");
    
    // Condition
    ExpressionPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for condition");
    
    // Update
    StatementPtr update = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        ExpressionPtr updateExpr = parseExpression();
        update = std::make_unique<ExpressionStatement>(std::move(updateExpr));
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' after for clauses");
    
    StatementPtr body = parseStatement();
    
    return std::make_unique<ForStatement>(
        std::move(init), std::move(condition), std::move(update), std::move(body));
}

StatementPtr Parser::parseWhileStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    ExpressionPtr condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition");
    
    StatementPtr body = parseStatement();
    
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

StatementPtr Parser::parseReturnStatement() {
    ExpressionPtr value = nullptr;
    
    if (!check(TokenType::SEMICOLON)) {
        value = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    return std::make_unique<ReturnStatement>(std::move(value));
}

ExpressionPtr Parser::parseExpression() {
    return parseLogicalOr();
}

ExpressionPtr Parser::parseLogicalOr() {
    ExpressionPtr expr = parseLogicalAnd();
    
    while (match(TokenType::LOGICAL_OR)) {
        BinaryExpression::Operator op = BinaryExpression::Operator::LOGICAL_OR;
        ExpressionPtr right = parseLogicalAnd();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

ExpressionPtr Parser::parseLogicalAnd() {
    ExpressionPtr expr = parseEquality();
    
    while (match(TokenType::LOGICAL_AND)) {
        BinaryExpression::Operator op = BinaryExpression::Operator::LOGICAL_AND;
        ExpressionPtr right = parseEquality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

ExpressionPtr Parser::parseEquality() {
    ExpressionPtr expr = parseComparison();
    
    while (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL)) {
        BinaryExpression::Operator op = (tokens_[current_ - 1].type == TokenType::EQUAL) ?
            BinaryExpression::Operator::EQUAL : BinaryExpression::Operator::NOT_EQUAL;
        ExpressionPtr right = parseComparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

ExpressionPtr Parser::parseComparison() {
    ExpressionPtr expr = parseAddition();
    
    while (match(TokenType::LESS_THAN) || match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER_THAN) || match(TokenType::GREATER_EQUAL)) {
        
        BinaryExpression::Operator op;
        TokenType tokenType = tokens_[current_ - 1].type;
        
        switch (tokenType) {
            case TokenType::LESS_THAN: op = BinaryExpression::Operator::LESS_THAN; break;
            case TokenType::LESS_EQUAL: op = BinaryExpression::Operator::LESS_EQUAL; break;
            case TokenType::GREATER_THAN: op = BinaryExpression::Operator::GREATER_THAN; break;
            case TokenType::GREATER_EQUAL: op = BinaryExpression::Operator::GREATER_EQUAL; break;
            default: throw std::runtime_error("Invalid comparison operator");
        }
        
        ExpressionPtr right = parseAddition();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

ExpressionPtr Parser::parseAddition() {
    ExpressionPtr expr = parseMultiplication();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        BinaryExpression::Operator op = (tokens_[current_ - 1].type == TokenType::PLUS) ?
            BinaryExpression::Operator::ADD : BinaryExpression::Operator::SUBTRACT;
        ExpressionPtr right = parseMultiplication();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

ExpressionPtr Parser::parseMultiplication() {
    ExpressionPtr expr = parseUnary();
    
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
        BinaryExpression::Operator op;
        TokenType tokenType = tokens_[current_ - 1].type;
        
        switch (tokenType) {
            case TokenType::MULTIPLY: op = BinaryExpression::Operator::MULTIPLY; break;
            case TokenType::DIVIDE: op = BinaryExpression::Operator::DIVIDE; break;
            case TokenType::MODULO: op = BinaryExpression::Operator::MODULO; break;
            default: throw std::runtime_error("Invalid multiplication operator");
        }
        
        ExpressionPtr right = parseUnary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

ExpressionPtr Parser::parseUnary() {
    if (match(TokenType::LOGICAL_NOT) || match(TokenType::MINUS)) {
        UnaryExpression::Operator op = (tokens_[current_ - 1].type == TokenType::LOGICAL_NOT) ?
            UnaryExpression::Operator::LOGICAL_NOT : UnaryExpression::Operator::MINUS;
        ExpressionPtr operand = parseUnary();
        return std::make_unique<UnaryExpression>(op, std::move(operand));
    }
    
    return parsePostfix();
}

ExpressionPtr Parser::parsePrimary() {
    if (match(TokenType::INTEGER_LITERAL)) {
        return std::make_unique<LiteralExpression>(
            LiteralExpression::LiteralType::INT, tokens_[current_ - 1].value);
    }
    
    if (match(TokenType::FLOAT_LITERAL)) {
        return std::make_unique<LiteralExpression>(
            LiteralExpression::LiteralType::FLOAT, tokens_[current_ - 1].value);
    }
    
    if (match(TokenType::STRING_LITERAL)) {
        return std::make_unique<LiteralExpression>(
            LiteralExpression::LiteralType::STRING, tokens_[current_ - 1].value);
    }
    
    if (match(TokenType::IDENTIFIER)) {
        std::string name = tokens_[current_ - 1].value;
        return std::make_unique<IdentifierExpression>(name);
    }
    
    // Handle type names that can be used as function names (constructors)
    if (match(TokenType::VEC2) || match(TokenType::VEC3) || match(TokenType::VEC4) ||
        match(TokenType::MAT2) || match(TokenType::MAT3) || match(TokenType::MAT4) ||
        match(TokenType::BOOL) || match(TokenType::INT) || match(TokenType::FLOAT)) {
        std::string name = tokens_[current_ - 1].value;
        return std::make_unique<IdentifierExpression>(name);
    }
    
    if (match(TokenType::LEFT_PAREN)) {
        ExpressionPtr expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw std::runtime_error("Expected expression");
}

ExpressionPtr Parser::parsePostfix() {
    ExpressionPtr expr = parsePrimary();
    
    while (true) {
        if (match(TokenType::DOT)) {
            Token member = consume(TokenType::IDENTIFIER, "Expected property name after '.'");
            expr = std::make_unique<MemberAccessExpression>(std::move(expr), member.value);
        } else if (match(TokenType::LEFT_PAREN)) {
            // Function call
            auto funcCall = std::make_unique<FunctionCallExpression>("");
            // Set the function name from the identifier expression
            if (auto identExpr = dynamic_cast<IdentifierExpression*>(expr.get())) {
                funcCall->functionName = identExpr->name;
            }
            
            // Parse arguments
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    funcCall->arguments.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            
            consume(TokenType::RIGHT_PAREN, "Expected ')' after function arguments");
            expr = std::move(funcCall);
        } else if (match(TokenType::LEFT_BRACKET)) {
            // Array access
            ExpressionPtr index = parseExpression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']' after array index");
            expr = std::make_unique<MemberAccessExpression>(std::move(expr), "[" + std::to_string(0) + "]");
        } else {
            break;
        }
    }
    
    return expr;
}

ExpressionPtr Parser::parseFunctionCall(const std::string& name) {
    // Stub implementation
    return nullptr;
}

TypePtr Parser::parseType() {
    if (match(TokenType::VOID)) {
        return std::make_unique<Type>(Type::Kind::VOID);
    } else if (match(TokenType::BOOL)) {
        return std::make_unique<Type>(Type::Kind::BOOL);
    } else if (match(TokenType::INT)) {
        return std::make_unique<Type>(Type::Kind::INT);
    } else if (match(TokenType::FLOAT)) {
        return std::make_unique<Type>(Type::Kind::FLOAT);
    } else if (match(TokenType::VEC2)) {
        return std::make_unique<Type>(Type::Kind::VEC2);
    } else if (match(TokenType::VEC3)) {
        return std::make_unique<Type>(Type::Kind::VEC3);
    } else if (match(TokenType::VEC4)) {
        return std::make_unique<Type>(Type::Kind::VEC4);
    } else if (match(TokenType::MAT2)) {
        return std::make_unique<Type>(Type::Kind::MAT2);
    } else if (match(TokenType::MAT3)) {
        return std::make_unique<Type>(Type::Kind::MAT3);
    } else if (match(TokenType::MAT4)) {
        return std::make_unique<Type>(Type::Kind::MAT4);
    } else if (match(TokenType::SAMPLER2D)) {
        return std::make_unique<Type>(Type::Kind::SAMPLER2D);
    } else if (match(TokenType::SAMPLER3D)) {
        return std::make_unique<Type>(Type::Kind::SAMPLER3D);
    } else if (match(TokenType::SAMPLERCUBE)) {
        return std::make_unique<Type>(Type::Kind::SAMPLERCUBE);
    }
    
    throw std::runtime_error("Expected type");
}

VariableDeclaration::Qualifier Parser::parseQualifier() {
    if (check(TokenType::IN)) {
        advance();
        return VariableDeclaration::Qualifier::IN;
    } else if (check(TokenType::OUT)) {
        advance();
        return VariableDeclaration::Qualifier::OUT;
    } else if (check(TokenType::UNIFORM)) {
        advance();
        return VariableDeclaration::Qualifier::UNIFORM;
    } else if (check(TokenType::CONST)) {
        advance();
        return VariableDeclaration::Qualifier::CONST;
    }
    return VariableDeclaration::Qualifier::NONE;
}

ShaderDeclaration::ShaderType Parser::parseShaderType() {
    if (match(TokenType::VERTEX)) {
        return ShaderDeclaration::ShaderType::VERTEX;
    } else if (match(TokenType::FRAGMENT)) {
        return ShaderDeclaration::ShaderType::FRAGMENT;
    } else if (match(TokenType::COMPUTE)) {
        return ShaderDeclaration::ShaderType::COMPUTE;
    }
    
    throw std::runtime_error("Expected shader type (vertex, fragment, or compute)");
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (tokens_[current_ - 1].type == TokenType::SEMICOLON) return;
        
        switch (currentToken().type) {
            case TokenType::SHADER:
            case TokenType::IF:
            case TokenType::FOR:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

void Parser::reportError(const std::string& message) {
    // For now, just print the error
    // In a real implementation, you'd collect errors for later reporting
    if (!isAtEnd()) {
        auto& token = currentToken();
        printf("Error at line %zu, column %zu: %s\n", token.line, token.column, message.c_str());
    } else {
        printf("Error at end of file: %s\n", message.c_str());
    }
}

} // namespace sdl
