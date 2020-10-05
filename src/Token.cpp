#include <string>
#include <vector>
#include <utility>
#include <Token.hpp>

using namespace kisslang;

SymbolToken::SymbolToken(
        SymbolTokenType type, std::string value,
        std::pair<int, int> position) :
        type(type), value(value), position(position) {
}

CompoundToken::CompoundToken(
        CompoundTokenType type, std::vector<Token> children) :
        type(type), children(children) {
}
