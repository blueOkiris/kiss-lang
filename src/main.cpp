#include <iostream>
#include <string>
#include <Io.hpp>
#include <Error.hpp>
#include <Parser.hpp>
#include <Compiler.hpp>

using namespace kisslang;

inline const int compileSourceCode(
        const std::string &source, const std::string &moduleName) {
    std::cout << "Compiling module " << moduleName << "." << std::endl;
    try {
        const auto tokens = Parser::lexTokens(source);
        const auto ast = Parser::parseAst(tokens);
        std::cout << ast.str() << std::endl;
        const auto cppCode = Compiler::generateCode(ast);
        Compiler::compile(moduleName, cppCode);
    } catch(const UnknownTokenException &ute) {
        std::cout << ute.what() << std::endl;
        return -1;
    } catch(const UnexpectedTokenException &ute) {
        std::cout << ute.what() << std::endl;
        return -1;
    }
    return 0;
}

int main(int argc, char **args) {
    std::string sourceCode;
    std::string moduleName;
    if(Io::sourceCodeFromArgs(sourceCode, moduleName, argc, args)) {
        return -1;
    }
    return compileSourceCode(sourceCode, moduleName);
}
