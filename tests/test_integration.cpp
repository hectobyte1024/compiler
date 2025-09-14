#include <gtest/gtest.h>
#include "compiler/compiler.h"
#include <fstream>
#include <cstdio>

using namespace sdl;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test SDL file
        std::ofstream file("test_input.sdl");
        file << R"(
            shader test : vertex {
            }
        )";
        file.close();
    }
    
    void TearDown() override {
        // Clean up test files
        std::remove("test_input.sdl");
        std::remove("test_output.glsl");
        std::remove("test_output.cu");
    }
};

TEST_F(IntegrationTest, CompileToGLSL) {
    Compiler compiler;
    CompilerOptions options;
    options.inputFile = "test_input.sdl";
    options.targets = {TargetLanguage::GLSL};
    
    bool success = compiler.compile(options);
    EXPECT_TRUE(success);
    
    if (!success) {
        for (const auto& error : compiler.getErrors()) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    std::string output = compiler.getGLSLOutput();
    EXPECT_FALSE(output.empty());
}

TEST_F(IntegrationTest, CompileToCUDA) {
    Compiler compiler;
    CompilerOptions options;
    options.inputFile = "test_input.sdl";
    options.targets = {TargetLanguage::CUDA};
    
    bool success = compiler.compile(options);
    EXPECT_TRUE(success);
    
    std::string output = compiler.getCUDAOutput();
    EXPECT_FALSE(output.empty());
}
