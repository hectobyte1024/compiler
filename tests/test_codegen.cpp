#include <gtest/gtest.h>
#include "codegen/glsl_generator.h"
#include "codegen/cuda_generator.h"
#include "parser/ast.h"

using namespace sdl;

class CodegenTest : public ::testing::Test {
protected:
    std::unique_ptr<Program> createSimpleProgram() {
        auto program = std::make_unique<Program>();
        
        auto shader = std::make_unique<ShaderDeclaration>("test", ShaderDeclaration::ShaderType::VERTEX);
        program->declarations.push_back(std::move(shader));
        
        return program;
    }
};

TEST_F(CodegenTest, GLSLGeneration) {
    auto program = createSimpleProgram();
    
    GLSLGenerator generator;
    std::string output = generator.generate(*program);
    
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("#version"), std::string::npos);
}

TEST_F(CodegenTest, CUDAGeneration) {
    auto program = createSimpleProgram();
    
    CUDAGenerator generator;
    std::string output = generator.generate(*program);
    
    EXPECT_FALSE(output.empty());
    EXPECT_NE(output.find("#include <cuda_runtime.h>"), std::string::npos);
}
