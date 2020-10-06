#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <Token.hpp>

using namespace kisslang;

std::string Token::str() const {
    return "Token";
}

SymbolToken::SymbolToken(
        const SymbolTokenType &type, const std::string &value,
        const std::pair<int, int> &position) :
        type(type), value(value), position(position) {
}

std::string SymbolToken::str() const {
    std::stringstream tokenStr;
    tokenStr << "{ " << static_cast<char>(type) << " : " << value << " }";
    return tokenStr.str();
}

CompoundToken::CompoundToken(
        const CompoundTokenType &newType, const std::vector<Token> &tokens) :
        type(newType), children(tokens) {
}

std::string CompoundToken::str() const {
    std::stringstream tokenStr;
    tokenStr << "{ " << static_cast<char>(type) << " : [ ";
    for(auto it = children.begin(); it != children.end(); ++it) {
        tokenStr << (*it).str();
        if(it != children.end()) {
            tokenStr << ", ";
        }
    }
    tokenStr << " ] }";
    return tokenStr.str();
}
