#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <sys/stat.h>
#include <Io.hpp>

using namespace kisslang;

inline const bool isRegularFile(const std::string &path) {
    struct stat pathStat;
    stat(path.c_str(), &pathStat);
    return S_ISREG(pathStat.st_mode);
}

inline const std::string replace(
        const std::string src, const std::string what, const std::string with) {
    size_t startPos = src.find(what);
    if(startPos == std::string::npos) {
        return src;
    }
    auto newStr = std::string(src);
    newStr.replace(startPos, what.length(), with);
    return newStr;
}

const int Io::sourceCodeFromArgs(
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
    bool isFile = srcFile.good() && isRegularFile(args[1]);
    bool isDir = false;
    if(!isFile) {
        srcFile.close();
        
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
        moduleName = std::string(args[1]);
    }
    code = src;
    
    return 0;
}
