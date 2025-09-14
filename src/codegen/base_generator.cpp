#include "codegen/base_generator.h"

namespace sdl {

std::string BaseCodeGenerator::generate(Program& program) {
    output_.str("");
    output_.clear();
    
    generatePreamble();
    program.accept(*this);
    generatePostamble();
    
    return output_.str();
}

void BaseCodeGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) {
        output_ << "    ";
    }
}

void BaseCodeGenerator::writeLine(const std::string& line) {
    indent();
    output_ << line << "\n";
}

void BaseCodeGenerator::write(const std::string& text) {
    output_ << text;
}

void BaseCodeGenerator::increaseIndent() {
    indentLevel_++;
}

void BaseCodeGenerator::decreaseIndent() {
    if (indentLevel_ > 0) {
        indentLevel_--;
    }
}

} // namespace sdl
