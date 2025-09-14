#include "cli/cli_parser.h"
#include <iostream>
#include <sstream>

namespace sdl {

CLIParser::Options CLIParser::parse(int argc, char* argv[]) {
    Options options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
        } else if (arg == "-v" || arg == "--version") {
            options.showVersion = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "-t" || arg == "--target") {
            if (i + 1 < argc) {
                std::string targets = argv[++i];
                options.targets = splitString(targets, ',');
            } else {
                throw std::runtime_error("Missing argument for " + arg);
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                throw std::runtime_error("Missing argument for " + arg);
            }
        } else if (arg == "-I" || arg == "--include") {
            if (i + 1 < argc) {
                options.includePaths.push_back(argv[++i]);
            } else {
                throw std::runtime_error("Missing argument for " + arg);
            }
        } else if (arg == "-D" || arg == "--define") {
            if (i + 1 < argc) {
                options.defines.push_back(argv[++i]);
            } else {
                throw std::runtime_error("Missing argument for " + arg);
            }
        } else if (arg.front() == '-') {
            throw std::runtime_error("Unknown option: " + arg);
        } else {
            // Input file
            if (options.inputFile.empty()) {
                options.inputFile = arg;
            } else {
                throw std::runtime_error("Multiple input files not supported");
            }
        }
    }
    
    return options;
}

void CLIParser::printHelp() {
    std::cout << "SDL Shader Compiler v1.0.0\n";
    std::cout << "Usage: sdl_compiler [options] <input_file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -t, --target <target>     Target language (glsl, cuda, or both)\n";
    std::cout << "  -o, --output <file>       Output file name\n";
    std::cout << "  -I, --include <dir>       Add include directory\n";
    std::cout << "  -D, --define <macro>      Define preprocessor macro\n";
    std::cout << "  --verbose                 Enable verbose output\n";
    std::cout << "  -h, --help                Show this help message\n";
    std::cout << "  -v, --version             Show version information\n\n";
    std::cout << "Examples:\n";
    std::cout << "  sdl_compiler shader.sdl                    # Compile to GLSL\n";
    std::cout << "  sdl_compiler -t cuda shader.sdl           # Compile to CUDA\n";
    std::cout << "  sdl_compiler -t glsl,cuda shader.sdl      # Compile to both\n";
    std::cout << "  sdl_compiler -o output.glsl shader.sdl    # Specify output file\n";
}

void CLIParser::printVersion() {
    std::cout << "SDL Shader Compiler v1.0.0\n";
    std::cout << "A domain-specific language compiler for GLSL and CUDA\n";
}

std::vector<std::string> CLIParser::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

} // namespace sdl
