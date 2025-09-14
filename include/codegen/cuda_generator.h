#pragma once

#include "codegen/base_generator.h"

namespace sdl {

class CUDAGenerator : public BaseCodeGenerator {
public:
    CUDAGenerator();
    
    // ASTVisitor interface
    void visit(Type& node) override;
    void visit(IdentifierExpression& node) override;
    void visit(LiteralExpression& node) override;
    void visit(BinaryExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(FunctionCallExpression& node) override;
    void visit(MemberAccessExpression& node) override;
    void visit(ExpressionStatement& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(VariableDeclaration& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(ShaderDeclaration& node) override;
    void visit(BlockStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(Program& node) override;
    
protected:
    std::string getTypeString(const Type& type) override;
    std::string getQualifierString(VariableDeclaration::Qualifier qualifier) override;
    std::string getBinaryOperatorString(BinaryExpression::Operator op) override;
    std::string getUnaryOperatorString(UnaryExpression::Operator op) override;
    std::string getFunctionCallString(const std::string& name, 
                                     const std::vector<std::string>& args) override;
    
    void generatePreamble() override;
    void generatePostamble() override;
    
private:
    bool inKernel_ = false;
    
    void generateCUDAIncludes();
    std::string getCUDABuiltinFunction(const std::string& name);
    std::string mapGLSLTypeToCUDA(const std::string& glslType);
    std::string generateKernelSignature(const ShaderDeclaration& shader);
};

} // namespace sdl
