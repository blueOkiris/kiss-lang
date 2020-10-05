#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <Parser.hpp>
#include <Compiler.hpp>

using namespace kisslang;

const int sourceCodeFromArgs(
    std::string &code, std::string &moduleName, int argc, char **args
);
const int compileSource(
    const std::string &source, const std::string &moduleName
);
const std::string replace(
    const std::string src, const std::string what, const std::string with
);

int main(int argc, char **args) {
    std::string sourceCode;
    std::string moduleName;
    if(!sourceCodeFromArgs(sourceCode, moduleName, argc, args)) {
        return -1;
    }
    return compileSource(sourceCode, moduleName);
}

const int compileSource(
        const std::string &source, const std::string &moduleName) {
    try {
        const auto tokens = Parser::lexTokens(source);
        const auto ast = Parser::parseAst(tokens);
        const auto cppCode = Compiler::generateCode(ast);
        Compiler::compile(moduleName, cppCode);
    } catch(...) { // Will become more specified as actual errors are developed
        std::cout << "An unknown error occurred." << std::endl;
        return -1;
    }
    return 0;
}

const int sourceCodeFromArgs(
        std::string &code, std::string &moduleName,
        int argc, char **args) {
    // Check for file/module name given
    if(argc < 2) {
        std::cout << "No file name provided." << std::endl;
        moduleName = "";
        code = "";
        return -1;
    }
    
    if(argc > 2) {
        std::cout << "Too many arguments given!" << std::endl;
        moduleName = "";
        code = "";
        return -1;
    }
    
    // Check if a valid file name or module name
    std::ifstream srcFile(args[1]);
    bool isFile = srcFile.good();
    bool isDir = false;
    if(!isFile) {
        std::stringstream moduleFileName;
        moduleFileName << args[1] << "/main.kiss";
        srcFile.open(moduleFileName.str());
        isDir = srcFile.good();
    }
    if(!isFile && !isDir) {
        std::cout << "Not a valid filename: " << args[1] << std::endl;
        moduleName = "";
        code = "";
        return -1;
    }
    
    // Get our source code
    // Note that in testing if file exists we have successfully opened a file!
    std::string src;
    srcFile.seekg(0, std::ios::end);
    src.reserve(srcFile.tellg());
    srcFile.seekg(0, std::ios::beg);
    src.assign(
        std::istreambuf_iterator<char>(srcFile),
        std::istreambuf_iterator<char>()
    );
    
    if(isFile) {
        moduleName = replace(std::string(args[1]), ".kiss", "");
    } else {
        moduleName = std::string(args[1]) + ".kiss";
    }
    code = src;
    return 0;
}

const std::string replace(
        const std::string src, const std::string what, const std::string with) {
    size_t startPos = src.find(what);
    if(startPos == std::string::npos) {
        return src;
    }
    auto newStr = std::string(src);
    newStr.replace(startPos, what.length(), with);
    return newStr;
}
