#pragma once

#include <memory>
#include <string>
#include <vector>
#include <regex>
#include <Error.hpp>
#include <Token.hpp>

namespace kisslang {
    inline bool shouldSkipSpaces(const char &chr, int &lineRef, int &colRef) {
        if(chr == ' ' || chr == '\t' || chr == '\r') {
            colRef++;
            return true;
        } else if(chr == '\n') {
            lineRef++;
            colRef = 1;
            return true;
        }
        return false;
    }

    // The following two are the same idea, but work a bit differently with state
    inline const std::string matchRegEx(
            const std::string &regexStr, const std::string &currStr,
            bool &foundMatches) {
        std::smatch matches;
        const std::regex currRegex(regexStr);
        foundMatches = std::regex_search(currStr, matches, currRegex);
        return matches[0].str();
    }

    inline const bool matchRegex(
            const std::string &regexStr, const std::string &currStr,
            std::string &matchStr) {
        std::smatch matches;
        const std::regex currRegex(regexStr);
        bool foundMatches = std::regex_search(currStr, matches, currRegex);
        matchStr = matches[0].str();
        return foundMatches;
    }

    inline const bool startswith(const std::string &str, const std::string &what) {
        return str.rfind(what, 0) == 0;
    }

    inline const long int nonStatementIndex(const std::string &str) {
        for(long int ind = 0; ind < static_cast<long int>(str.length()); ind++) {
            const auto chr = str[ind];
            if(chr != 't' && chr != 'F' && chr != 'L' && chr != 'a' && chr != '='
                    && chr != 'd' && chr != 'n') {
                return ind;
            }
        }
        return -1;
    }

    inline void replaceTokens(
            std::vector<std::shared_ptr<Token>> &tokenTree,
            const std::size_t &start, const std::size_t &end,
            const std::shared_ptr<CompoundToken> &newTokenPtr) {
        tokenTree.insert(
            tokenTree.begin() + start,
            std::dynamic_pointer_cast<Token>(newTokenPtr)
        );
        tokenTree.erase(
            tokenTree.begin() + start + 1,
            tokenTree.begin() + end + 1
        );
    }

    inline void throwUnexpectedTokenException(
            const std::string &currTreeStr,
            const std::vector<std::shared_ptr<Token>> &tokenTree) {
        std::shared_ptr<Token> currToken = tokenTree.at(
            nonStatementIndex(currTreeStr)
        );
        while(!currToken->isSymbol()) {
            currToken = std::dynamic_pointer_cast<CompoundToken>(
                currToken
            )->children[0];
        }
        auto symbolPtr = std::dynamic_pointer_cast<SymbolToken>(currToken);
        const auto symbol = *symbolPtr;
        throw UnexpectedTokenException(symbol, currTreeStr);
    }

    inline void copySymbolTokensToParentPtrs(
            std::vector<std::shared_ptr<Token>> &dest,
            const std::vector<SymbolToken> &src) {
        for(auto tokenIt = src.begin(); tokenIt != src.end(); ++tokenIt) {
            const auto symbolPtr = std::make_shared<SymbolToken>(*tokenIt);
            dest.push_back(std::dynamic_pointer_cast<Token>(symbolPtr));
        }
    }
}
