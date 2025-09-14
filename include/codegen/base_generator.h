#pragma once

#include "parser/ast.h"
#include "parser/ast_visitor.h"
#include <string>
#include <sstream>

namespace sdl {

class BaseCodeGenerator : public ASTVisitor {
public:
    virtual ~BaseCodeGenerator() = default;
    
    std::string generate(Program& program);
    
protected:
    std::stringstream output_;
    int indentLevel_ = 0;
    
    void indent();
    void writeLine(const std::string& line = "");
    void write(const std::string& text);
    void increaseIndent();
    void decreaseIndent();
    
    // Helper methods for different targets
    virtual std::string getTypeString(const Type& type) = 0;
    virtual std::string getQualifierString(VariableDeclaration::Qualifier qualifier) = 0;
    virtual std::string getBinaryOperatorString(BinaryExpression::Operator op) = 0;
    virtual std::string getUnaryOperatorString(UnaryExpression::Operator op) = 0;
    
    // Target-specific code generation hooks
    virtual void generatePreamble() = 0;
    virtual void generatePostamble() = 0;
    virtual std::string getFunctionCallString(const std::string& name, 
                                             const std::vector<std::string>& args) = 0;
};

} // namespace sdl
