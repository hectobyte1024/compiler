#pragma once

#include <string>

namespace sdl {

class SymbolTable {
public:
    void enterScope();
    void exitScope();
    void define(const std::string& name, const std::string& type);
    bool lookup(const std::string& name);
};

} // namespace sdl
