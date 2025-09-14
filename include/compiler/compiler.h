#pragma once

#include <string>
#include <vector>
#include <memory>

namespace sdl {

enum class TargetLanguage {
    GLSL,
    CUDA
};

struct CompilerOptions {
    std::vector<TargetLanguage> targets;
    std::string inputFile;
    std::string outputFile;
    std::vector<std::string> includePaths;
    std::vector<std::string> defines;
    bool verbose = false;
    bool optimizeOutput = true;
};

class Compiler {
public:
    Compiler();
    ~Compiler();
    
    bool compile(const CompilerOptions& options);
    
    // Get compilation results
    std::string getGLSLOutput() const;
    std::string getCUDAOutput() const;
    
    // Error handling
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace sdl
