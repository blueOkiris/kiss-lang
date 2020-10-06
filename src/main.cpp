#include <iostream>
#include <string>
#include <Io.hpp>
#include <Error.hpp>
#include <Parser.hpp>
#include <Compiler.hpp>

using namespace kisslang;

inline void compileSourceCode(
        const std::string &source, const std::string &moduleName) {
    const auto tokens = Parser::lexTokens(source);
    const auto ast = Parser::parseAst(tokens);    
    const auto cppCode = Compiler::generateCode(ast);
    Compiler::compile(moduleName, cppCode);
    
    std::cout << ast.str() << std::endl;
}

int main(int argc, char **args) {
    std::string sourceCode;
    std::string moduleName;
    if(Io::sourceCodeFromArgs(sourceCode, moduleName, argc, args) != 0) {
        return -1;
    }
    
    std::cout << "Compiling module " << moduleName << "." << std::endl;
    try {
        compileSourceCode(sourceCode, moduleName);
    } catch(const UnknownTokenException &ute) {
        std::cout << ute.what() << std::endl;
        return -1;
    } catch(const UnexpectedTokenException &ute) {
        std::cout << ute.what() << std::endl;
        return -1;
    }
    
    return 0;
}
