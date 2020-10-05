#include <string>
#include <vector>
#include <utility>
#include <Token.hpp>

using namespace kisslang;

SymbolToken::SymbolToken(
        const SymbolTokenType &type, const std::string &value,
        const std::pair<int, int> &position) :
        type(type), value(value), position(position) {
}

CompoundToken::CompoundToken(
        const CompoundTokenType &type, const std::vector<Token> &children) :
        type(type), children(children) {
}
