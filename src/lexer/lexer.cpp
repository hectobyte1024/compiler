#include "lexer/lexer.h"
#include <unordered_map>
#include <cctype>

namespace sdl {

static const std::unordered_map<std::string, TokenType> keywords = {
    {"shader", TokenType::SHADER},
    {"vertex", TokenType::VERTEX},
    {"fragment", TokenType::FRAGMENT},
    {"compute", TokenType::COMPUTE},
    {"in", TokenType::IN},
    {"out", TokenType::OUT},
    {"uniform", TokenType::UNIFORM},
    {"const", TokenType::CONST},
    {"struct", TokenType::STRUCT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},
    {"void", TokenType::VOID},
    {"bool", TokenType::BOOL},
    {"int", TokenType::INT},
    {"float", TokenType::FLOAT},
    {"vec2", TokenType::VEC2},
    {"vec3", TokenType::VEC3},
    {"vec4", TokenType::VEC4},
    {"mat2", TokenType::MAT2},
    {"mat3", TokenType::MAT3},
    {"mat4", TokenType::MAT4},
    {"sampler2D", TokenType::SAMPLER2D},
    {"sampler3D", TokenType::SAMPLER3D},
    {"samplerCube", TokenType::SAMPLERCUBE}
};

Lexer::Lexer(const std::string& source) 
    : source_(source), position_(0), line_(1), column_(1) {
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::WHITESPACE && token.type != TokenType::COMMENT) {
            tokens.push_back(token);
        }
    }
    
    tokens.emplace_back(TokenType::END_OF_FILE, "", line_, column_);
    return tokens;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    size_t tokenLine = line_;
    size_t tokenColumn = column_;
    
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE, "", tokenLine, tokenColumn);
    }
    
    char c = currentChar();
    
    // Single character tokens
    switch (c) {
        case '(': advance(); return Token(TokenType::LEFT_PAREN, "(", tokenLine, tokenColumn);
        case ')': advance(); return Token(TokenType::RIGHT_PAREN, ")", tokenLine, tokenColumn);
        case '{': advance(); return Token(TokenType::LEFT_BRACE, "{", tokenLine, tokenColumn);
        case '}': advance(); return Token(TokenType::RIGHT_BRACE, "}", tokenLine, tokenColumn);
        case '[': advance(); return Token(TokenType::LEFT_BRACKET, "[", tokenLine, tokenColumn);
        case ']': advance(); return Token(TokenType::RIGHT_BRACKET, "]", tokenLine, tokenColumn);
        case ';': advance(); return Token(TokenType::SEMICOLON, ";", tokenLine, tokenColumn);
        case ':': advance(); return Token(TokenType::COLON, ":", tokenLine, tokenColumn);
        case ',': advance(); return Token(TokenType::COMMA, ",", tokenLine, tokenColumn);
        case '.': advance(); return Token(TokenType::DOT, ".", tokenLine, tokenColumn);
        case '+': advance(); return Token(TokenType::PLUS, "+", tokenLine, tokenColumn);
        case '-': advance(); return Token(TokenType::MINUS, "-", tokenLine, tokenColumn);
        case '*': advance(); return Token(TokenType::MULTIPLY, "*", tokenLine, tokenColumn);
        case '%': advance(); return Token(TokenType::MODULO, "%", tokenLine, tokenColumn);
    }
    
    // Two character tokens
    if (c == '=') {
        advance();
        if (currentChar() == '=') {
            advance();
            return Token(TokenType::EQUAL, "==", tokenLine, tokenColumn);
        }
        return Token(TokenType::ASSIGN, "=", tokenLine, tokenColumn);
    }
    
    if (c == '!') {
        advance();
        if (currentChar() == '=') {
            advance();
            return Token(TokenType::NOT_EQUAL, "!=", tokenLine, tokenColumn);
        }
        return Token(TokenType::LOGICAL_NOT, "!", tokenLine, tokenColumn);
    }
    
    if (c == '<') {
        advance();
        if (currentChar() == '=') {
            advance();
            return Token(TokenType::LESS_EQUAL, "<=", tokenLine, tokenColumn);
        }
        return Token(TokenType::LESS_THAN, "<", tokenLine, tokenColumn);
    }
    
    if (c == '>') {
        advance();
        if (currentChar() == '=') {
            advance();
            return Token(TokenType::GREATER_EQUAL, ">=", tokenLine, tokenColumn);
        }
        return Token(TokenType::GREATER_THAN, ">", tokenLine, tokenColumn);
    }
    
    if (c == '&') {
        advance();
        if (currentChar() == '&') {
            advance();
            return Token(TokenType::LOGICAL_AND, "&&", tokenLine, tokenColumn);
        }
    }
    
    if (c == '|') {
        advance();
        if (currentChar() == '|') {
            advance();
            return Token(TokenType::LOGICAL_OR, "||", tokenLine, tokenColumn);
        }
    }
    
    if (c == '/') {
        if (peekChar() == '/') {
            skipComment();
            return Token(TokenType::COMMENT, "", tokenLine, tokenColumn);
        }
        advance();
        return Token(TokenType::DIVIDE, "/", tokenLine, tokenColumn);
    }
    
    // String literals
    if (c == '"') {
        return readString();
    }
    
    // Numbers
    if (isDigit(c)) {
        return readNumber();
    }
    
    // Identifiers and keywords
    if (isAlpha(c)) {
        return readIdentifier();
    }
    
    // Unknown character
    advance();
    return Token(TokenType::UNKNOWN, std::string(1, c), tokenLine, tokenColumn);
}

char Lexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return source_[position_];
}

char Lexer::peekChar(size_t offset) const {
    size_t pos = position_ + offset;
    if (pos >= source_.length()) return '\0';
    return source_[pos];
}

void Lexer::advance() {
    if (isAtEnd()) return;
    
    if (source_[position_] == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    position_++;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(currentChar())) {
        advance();
    }
}

void Lexer::skipComment() {
    // Skip single-line comment
    while (!isAtEnd() && currentChar() != '\n') {
        advance();
    }
}

Token Lexer::readIdentifier() {
    size_t start = position_;
    size_t tokenLine = line_;
    size_t tokenColumn = column_;
    
    while (!isAtEnd() && isAlphaNumeric(currentChar())) {
        advance();
    }
    
    std::string value = source_.substr(start, position_ - start);
    TokenType type = getKeywordType(value);
    
    return Token(type, value, tokenLine, tokenColumn);
}

Token Lexer::readNumber() {
    size_t start = position_;
    size_t tokenLine = line_;
    size_t tokenColumn = column_;
    
    while (!isAtEnd() && isDigit(currentChar())) {
        advance();
    }
    
    // Check for decimal point
    if (!isAtEnd() && currentChar() == '.' && isDigit(peekChar())) {
        advance(); // consume '.'
        while (!isAtEnd() && isDigit(currentChar())) {
            advance();
        }
        
        std::string value = source_.substr(start, position_ - start);
        return Token(TokenType::FLOAT_LITERAL, value, tokenLine, tokenColumn);
    }
    
    std::string value = source_.substr(start, position_ - start);
    return Token(TokenType::INTEGER_LITERAL, value, tokenLine, tokenColumn);
}

Token Lexer::readString() {
    size_t start = position_;
    size_t tokenLine = line_;
    size_t tokenColumn = column_;
    
    advance(); // skip opening quote
    
    while (!isAtEnd() && currentChar() != '"') {
        if (currentChar() == '\\') {
            advance(); // skip escape character
        }
        advance();
    }
    
    if (isAtEnd()) {
        // Unterminated string
        return Token(TokenType::UNKNOWN, "", tokenLine, tokenColumn);
    }
    
    advance(); // skip closing quote
    
    // Extract string content (without quotes)
    std::string value = source_.substr(start + 1, position_ - start - 2);
    return Token(TokenType::STRING_LITERAL, value, tokenLine, tokenColumn);
}

TokenType Lexer::getKeywordType(const std::string& identifier) const {
    auto it = keywords.find(identifier);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isAtEnd() const {
    return position_ >= source_.length();
}

} // namespace sdl
