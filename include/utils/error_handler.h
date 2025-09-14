#pragma once

#include <string>

namespace sdl {

class ErrorHandler {
public:
    static void report(const std::string& message);
};

} // namespace sdl
