#pragma once

#include <string>
#include <Token.hpp>

/*
 * No need for class here.
 * These two methods would just be static lol
 * Just simple module encapsulation instead
 */
namespace kisslang {
    namespace Compiler {
        const std::string generateCode(const CompoundToken &ast);
        void compile(
            const std::string &outputFileName, const std::string &cppCode
        );
    }
}
