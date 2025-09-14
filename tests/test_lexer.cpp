#include <gtest/gtest.h>
#include "lexer/lexer.h"

using namespace sdl;

class LexerTest : public ::testing::Test {
protected:
    void tokenizeAndCheck(const std::string& source, 
                         const std::vector<TokenType>& expectedTypes) {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        ASSERT_EQ(tokens.size(), expectedTypes.size());
        
        for (size_t i = 0; i < expectedTypes.size(); ++i) {
            EXPECT_EQ(tokens[i].type, expectedTypes[i]) 
                << "Token " << i << " type mismatch";
        }
    }
};

TEST_F(LexerTest, TokenizeKeywords) {
    tokenizeAndCheck("shader vertex fragment", {
        TokenType::SHADER,
        TokenType::VERTEX,
        TokenType::FRAGMENT,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, TokenizeTypes) {
    tokenizeAndCheck("vec3 mat4 sampler2D", {
        TokenType::VEC3,
        TokenType::MAT4,
        TokenType::SAMPLER2D,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, TokenizeOperators) {
    tokenizeAndCheck("= + - * / == != < <= > >=", {
        TokenType::ASSIGN,
        TokenType::PLUS,
        TokenType::MINUS,
        TokenType::MULTIPLY,
        TokenType::DIVIDE,
        TokenType::EQUAL,
        TokenType::NOT_EQUAL,
        TokenType::LESS_THAN,
        TokenType::LESS_EQUAL,
        TokenType::GREATER_THAN,
        TokenType::GREATER_EQUAL,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, TokenizePunctuation) {
    tokenizeAndCheck("( ) { } [ ] ; : , .", {
        TokenType::LEFT_PAREN,
        TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::LEFT_BRACKET,
        TokenType::RIGHT_BRACKET,
        TokenType::SEMICOLON,
        TokenType::COLON,
        TokenType::COMMA,
        TokenType::DOT,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, TokenizeLiterals) {
    tokenizeAndCheck("123 3.14 \"hello\" identifier", {
        TokenType::INTEGER_LITERAL,
        TokenType::FLOAT_LITERAL,
        TokenType::STRING_LITERAL,
        TokenType::IDENTIFIER,
        TokenType::END_OF_FILE
    });
}

TEST_F(LexerTest, TokenizeSimpleShader) {
    std::string source = R"(
        shader main : vertex {
            in vec3 position;
            uniform mat4 mvp;
            void main() {
                gl_Position = mvp * vec4(position, 1.0);
            }
        }
    )";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    // Check that we have reasonable number of tokens
    EXPECT_GT(tokens.size(), 20);
    
    // Check first few tokens
    EXPECT_EQ(tokens[0].type, TokenType::SHADER);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].value, "main");
    EXPECT_EQ(tokens[2].type, TokenType::COLON);
    EXPECT_EQ(tokens[3].type, TokenType::VERTEX);
    EXPECT_EQ(tokens[4].type, TokenType::LEFT_BRACE);
}

TEST_F(LexerTest, HandleComments) {
    std::string source = "shader // this is a comment\nmain";
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::SHADER);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].value, "main");
}
