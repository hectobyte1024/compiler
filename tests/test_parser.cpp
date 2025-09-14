#include <gtest/gtest.h>
#include "parser/parser.h"
#include "lexer/lexer.h"

using namespace sdl;

class ParserTest : public ::testing::Test {
protected:
    std::unique_ptr<Program> parseString(const std::string& source) {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        return parser.parseProgram();
    }
};

TEST_F(ParserTest, ParseSimpleShader) {
    std::string source = R"(
        shader test : vertex {
        }
    )";
    
    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    EXPECT_EQ(program->declarations.size(), 1);
}

TEST_F(ParserTest, ParseEmptyProgram) {
    std::string source = "";
    
    auto program = parseString(source);
    ASSERT_NE(program, nullptr);
    EXPECT_EQ(program->declarations.size(), 0);
}
