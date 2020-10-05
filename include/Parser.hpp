#pragma once

#include <string>
#include <vector>
#include <Token.hpp>

/*
 * No need for class here.
 * These two methods would just be static lol
 * Just simple module encapsulation instead
 */
namespace kisslang {
    namespace Parser {
        const std::vector<SymbolToken> lexTokens(const std::string &code);
        const CompoundToken parseAst(const std::vector<SymbolToken> &tokens);
    }
}
