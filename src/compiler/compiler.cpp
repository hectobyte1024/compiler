#include "compiler/compiler.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "codegen/glsl_generator.h"
#include "codegen/cuda_generator.h"
#include <fstream>
#include <sstream>

namespace sdl {

class Compiler::Impl {
public:
    std::string glslOutput_;
    std::string cudaOutput_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    bool compile(const CompilerOptions& options) {
        try {
            // Read input file
            std::ifstream file(options.inputFile);
            if (!file) {
                errors_.push_back("Cannot open input file: " + options.inputFile);
                return false;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string source = buffer.str();
            
            if (options.verbose) {
                printf("Read %zu characters from %s\n", source.length(), options.inputFile.c_str());
            }
            
            // Lexical analysis
            Lexer lexer(source);
            auto tokens = lexer.tokenize();
            
            if (options.verbose) {
                printf("Generated %zu tokens\n", tokens.size());
            }
            
            // Parsing
            Parser parser(std::move(tokens));
            auto program = parser.parseProgram();
            
            if (!program) {
                errors_.push_back("Failed to parse program");
                return false;
            }
            
            if (options.verbose) {
                printf("Parsed %zu declarations\n", program->declarations.size());
            }
            
            // Code generation
            for (auto target : options.targets) {
                if (target == TargetLanguage::GLSL) {
                    GLSLGenerator generator;
                    glslOutput_ = generator.generate(*program);
                    
                    if (options.verbose) {
                        printf("Generated GLSL output (%zu characters)\n", glslOutput_.length());
                    }
                } else if (target == TargetLanguage::CUDA) {
                    CUDAGenerator generator;
                    cudaOutput_ = generator.generate(*program);
                    
                    if (options.verbose) {
                        printf("Generated CUDA output (%zu characters)\n", cudaOutput_.length());
                    }
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            errors_.push_back(e.what());
            return false;
        }
    }
};

Compiler::Compiler() : impl_(std::make_unique<Impl>()) {
}

Compiler::~Compiler() = default;

bool Compiler::compile(const CompilerOptions& options) {
    return impl_->compile(options);
}

std::string Compiler::getGLSLOutput() const {
    return impl_->glslOutput_;
}

std::string Compiler::getCUDAOutput() const {
    return impl_->cudaOutput_;
}

bool Compiler::hasErrors() const {
    return !impl_->errors_.empty();
}

std::vector<std::string> Compiler::getErrors() const {
    return impl_->errors_;
}

std::vector<std::string> Compiler::getWarnings() const {
    return impl_->warnings_;
}

} // namespace sdl
