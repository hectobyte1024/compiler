#include "cli/cli_parser.h"
#include "compiler/compiler.h"
#include <iostream>
#include <fstream>

using namespace sdl;

int main(int argc, char* argv[]) {
    try {
        auto options = CLIParser::parse(argc, argv);
        
        if (options.showHelp) {
            CLIParser::printHelp();
            return 0;
        }
        
        if (options.showVersion) {
            CLIParser::printVersion();
            return 0;
        }
        
        if (options.inputFile.empty()) {
            std::cerr << "Error: No input file specified\n";
            CLIParser::printHelp();
            return 1;
        }
        
        // Convert CLI options to compiler options
        CompilerOptions compilerOptions;
        compilerOptions.inputFile = options.inputFile;
        compilerOptions.outputFile = options.outputFile;
        compilerOptions.includePaths = options.includePaths;
        compilerOptions.defines = options.defines;
        compilerOptions.verbose = options.verbose;
        
        // Parse target languages
        for (const auto& target : options.targets) {
            if (target == "glsl") {
                compilerOptions.targets.push_back(TargetLanguage::GLSL);
            } else if (target == "cuda") {
                compilerOptions.targets.push_back(TargetLanguage::CUDA);
            } else {
                std::cerr << "Warning: Unknown target language '" << target << "'\n";
            }
        }
        
        if (compilerOptions.targets.empty()) {
            // Default to GLSL if no target specified
            compilerOptions.targets.push_back(TargetLanguage::GLSL);
        }
        
        // Create and run compiler
        Compiler compiler;
        
        if (options.verbose) {
            std::cout << "Compiling " << options.inputFile << "...\n";
        }
        
        bool success = compiler.compile(compilerOptions);
        
        if (!success) {
            std::cerr << "Compilation failed:\n";
            for (const auto& error : compiler.getErrors()) {
                std::cerr << "Error: " << error << "\n";
            }
            return 1;
        }
        
        // Output warnings
        for (const auto& warning : compiler.getWarnings()) {
            std::cerr << "Warning: " << warning << "\n";
        }
        
        // Write output files
        for (auto target : compilerOptions.targets) {
            std::string output;
            std::string extension;
            
            if (target == TargetLanguage::GLSL) {
                output = compiler.getGLSLOutput();
                extension = ".glsl";
            } else if (target == TargetLanguage::CUDA) {
                output = compiler.getCUDAOutput();
                extension = ".cu";
            }
            
            std::string outputFile = compilerOptions.outputFile;
            if (outputFile.empty()) {
                // Generate output filename from input
                size_t lastDot = compilerOptions.inputFile.find_last_of('.');
                if (lastDot != std::string::npos) {
                    outputFile = compilerOptions.inputFile.substr(0, lastDot);
                } else {
                    outputFile = compilerOptions.inputFile;
                }
            }
            
            if (compilerOptions.targets.size() > 1) {
                // Multiple targets, append target name
                std::string targetName = (target == TargetLanguage::GLSL) ? "_glsl" : "_cuda";
                outputFile += targetName;
            }
            
            outputFile += extension;
            
            std::ofstream outFile(outputFile);
            if (!outFile) {
                std::cerr << "Error: Cannot write to output file '" << outputFile << "'\n";
                return 1;
            }
            
            outFile << output;
            
            if (options.verbose) {
                std::cout << "Generated " << outputFile << "\n";
            }
        }
        
        if (options.verbose) {
            std::cout << "Compilation successful!\n";
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
