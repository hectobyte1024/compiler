#pragma once

#include <string>
#include <vector>

namespace sdl {

enum class TokenType {
    // Literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    
    // Keywords
    SHADER,
    VERTEX,
    FRAGMENT,
    COMPUTE,
    IN,
    OUT,
    UNIFORM,
    CONST,
    STRUCT,
    IF,
    ELSE,
    FOR,
    WHILE,
    RETURN,
    VOID,
    
    // Types
    BOOL,
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4,
    SAMPLER2D,
    SAMPLER3D,
    SAMPLERCUBE,
    
    // Operators
    ASSIGN,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    
    // Comparison
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    
    // Logical
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_NOT,
    
    // Punctuation
    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    
    // Special
    NEWLINE,
    WHITESPACE,
    COMMENT,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token(TokenType t, const std::string& v, size_t l, size_t c)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
public:
    explicit Lexer(const std::string& source);
    
    std::vector<Token> tokenize();
    Token nextToken();
    
private:
    std::string source_;
    size_t position_;
    size_t line_;
    size_t column_;
    
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipComment();
    bool isAtEnd() const;
    
    Token readIdentifier();
    Token readNumber();
    Token readString();
    
    TokenType getKeywordType(const std::string& identifier) const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
};

} // namespace sdl
