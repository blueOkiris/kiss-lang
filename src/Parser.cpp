#include <string>
#include <vector>
#include <Token.hpp>
#include <Parser.hpp>

using namespace kisslang;

const std::vector<SymbolToken> Parser::lexTokens(const std::string &code) {
    return std::vector<SymbolToken>();
}

const CompoundToken Parser::parseAst(const std::vector<SymbolToken> &tokens) {
    return CompoundToken(
        CompoundTokenType::None, std::vector<Token>()
    );
}
