#pragma once

#include <string>
#include <vector>

namespace sdl {

class CLIParser {
public:
    struct Options {
        std::vector<std::string> targets;
        std::string inputFile;
        std::string outputFile;
        std::vector<std::string> includePaths;
        std::vector<std::string> defines;
        bool verbose = false;
        bool showHelp = false;
        bool showVersion = false;
    };
    
    static Options parse(int argc, char* argv[]);
    static void printHelp();
    static void printVersion();
    
private:
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
};

} // namespace sdl
