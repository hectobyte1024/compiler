#pragma once

#include <string>

namespace sdl {

class FileUtils {
public:
    static std::string readFile(const std::string& filename);
    static bool writeFile(const std::string& filename, const std::string& content);
};

} // namespace sdl
