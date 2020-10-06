#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <memory>
#include <Token.hpp>

using namespace kisslang;

const std::string Token::tokenListAsTypeStr(
        const std::vector<std::shared_ptr<Token>> &tokens) {
    std::stringstream typeListStr;
    for(const auto &token : tokens) {
        if(token->isSymbol()) {
            const auto symbol = std::dynamic_pointer_cast<SymbolToken>(token);
            typeListStr << static_cast<char>(symbol->type);
        } else {
            const auto compound = std::dynamic_pointer_cast<CompoundToken>(
                token
            );
            typeListStr << static_cast<char>(compound->type);
        }
    }
    return typeListStr.str();
}

SymbolToken::SymbolToken(
        const SymbolTokenType &type, const std::string &value,
        const std::pair<int, int> &position) :
        type(type), value(value), position(position) {
}

const std::string SymbolToken::str() const {
    std::stringstream tokenStr;
    tokenStr << "{ " << static_cast<char>(type) << " : " << value << " }";
    return tokenStr.str();
}

const bool SymbolToken::isSymbol() const {
    return true;
}

CompoundToken::CompoundToken(
        const CompoundTokenType &newType,
        const std::vector<std::shared_ptr<Token>> &tokens) :
        type(newType), children(tokens) {
}

const std::string CompoundToken::str() const {
    std::stringstream tokenStr;
    tokenStr << "{ " << static_cast<char>(type) << " : [ ";
    for(auto it = children.begin(); it != children.end(); ++it) {
        tokenStr << (*it)->str();
        if(it != children.end()) {
            tokenStr << ", ";
        }
    }
    tokenStr << " ] }";
    return tokenStr.str();
}

const bool CompoundToken::isSymbol() const {
    return false;
}
