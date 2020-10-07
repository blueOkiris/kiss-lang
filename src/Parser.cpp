#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <utility>
#include <memory>
#include <Error.hpp>
#include <Token.hpp>
#include <Parser.hpp>

using namespace kisslang;

// Can't use map for these because it MUST be in the correct order
static const std::vector<std::pair<std::string, SymbolTokenType>>
        symbolRegexs = {
    { "loop|func|struct",                   SymbolTokenType::Keyword },
    { "true|false",                         SymbolTokenType::Boolean },
    {
        "([0-9]+|0x[0-9A-Za-z]+|0b[01]+):[1248]",
        SymbolTokenType::Integer
    },
    { "([0-9]+\\.[0-9]*|\\.[0-9]+):[48]",   SymbolTokenType::Float },
    { "0c(\\\\.|[^\\\\])",                  SymbolTokenType::Character },
    { "'(\\\\.|[^\\\\'])*'",                SymbolTokenType::String },
    { "#:[1248]|\\.:[48]|@|\\?",            SymbolTokenType::TypeChar },
    { "[A-Za-z_][A-Za-z_0-9]*",             SymbolTokenType::Identifier },
    { "\\(|\\)",                            SymbolTokenType::Parenth },
    { "\\[|\\]",                            SymbolTokenType::Bracket },
    { "\\{|\\}",                            SymbolTokenType::Brace },
    { "->",                                 SymbolTokenType::ReturnOp },
    { "<<|>>",                              SymbolTokenType::DoubleArrow },
    {
        "\\+|-|\\*|\\/|%|\\+\\+|--|==|!=|>|<|>=|<=|&&|\\|\\||!|&|\\^|~|=",
        SymbolTokenType::Operator
    },
    { "\\$",                                SymbolTokenType::Dollar },
    { "::",                                 SymbolTokenType::TypeOp },
    { "\\.",                                SymbolTokenType::MemberOp }
};
static const std::vector<std::pair<std::string, CompoundTokenType>> 
        compoundRegexs = {
    { "[bic'f]",                        CompoundTokenType::RawType },
    { "\\(tt\\(",                       CompoundTokenType::Tuple },
    { "\\[t+\\[",                       CompoundTokenType::List },
    { "\\(n\\(\\{t*\\{",                CompoundTokenType::Struct },
    { "n(\\.n)+",                       CompoundTokenType::StructAccess },
    { "@|\\$n|(\\(NN\\()|(\\[N\\[)",    CompoundTokenType::TypeName },
    { "kn:N>N\\}",                      CompoundTokenType::FuncDef },
    { "[r,lsS]" ,                       CompoundTokenType::Type },
    { "\\{[tFLa=nd]*\\{",               CompoundTokenType::Body },
    { "k\\}",                           CompoundTokenType::Loop },
    { "<N<",                            CompoundTokenType::Cast },
    { "k\\{(n:N)*\\{",                  CompoundTokenType::StructDef }
};

// Helper functions
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

inline const std::string matchRegEx(
        const std::string &regexStr, const std::string &currStr,
        bool &foundMatches) {
    std::smatch matches;
    const std::regex currRegex(regexStr);
    foundMatches = std::regex_search(currStr, matches, currRegex);
    return matches[0].str();
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

// Main parser functions
const std::vector<SymbolToken> Parser::lexTokens(const std::string &code) {
    std::vector<SymbolToken> tokens;
    int line = 1, col = 1;
    for(auto codeStrIt = code.begin(); codeStrIt != code.end(); ++codeStrIt) {
        if(shouldSkipSpaces(*codeStrIt, line, col)) {
            continue;
        }
        
        bool foundMatches = false;
        const auto currStr = std::string(codeStrIt, code.end());
        for(const auto &regexTokenPair : symbolRegexs) {
            const auto matchStr = matchRegEx(
                regexTokenPair.first, currStr,
                foundMatches
            );
            if(foundMatches && startswith(currStr, matchStr)) {
                tokens.push_back(
                    SymbolToken(regexTokenPair.second, matchStr, { line, col })
                );
                codeStrIt += matchStr.length() - 1;
                col += matchStr.length() - 1;
                break;
            }
        }
        
        if(!foundMatches) {
            throw UnknownTokenException(line, col);
        }
        col++;
    }
    return tokens;
}

const CompoundToken Parser::parseAst(const std::vector<SymbolToken> &tokens) {
    std::vector<std::shared_ptr<Token>> tokenTree;
    
    // Fill with copies of symbols to start
    for(auto tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
        const auto symbolPtr = std::make_shared<SymbolToken>(*tokenIt);
        tokenTree.push_back(std::dynamic_pointer_cast<Token>(symbolPtr));
    }
    
    auto currTreeStr = Token::tokenListAsTypeStr(tokenTree);
    while(nonStatementIndex(currTreeStr) != -1) {
        const auto oldStr = currTreeStr;
        
        for(const auto &regexTokenPair : compoundRegexs) {
            const std::regex currRegex(regexTokenPair.first);
            
            std::smatch matches;
            while(std::regex_search(currTreeStr, matches, currRegex)) {
                const auto matchStr = matches[0].str();
                const auto matchLocation = currTreeStr.find(matchStr);
                const auto matchEnd = matchLocation + matchStr.length();
                const auto newTokenPtr = std::make_shared<CompoundToken>(
                    CompoundToken(
                        regexTokenPair.second,
                        std::vector<std::shared_ptr<Token>>(
                            tokenTree.begin() + matchLocation,
                            tokenTree.begin() + matchEnd
                        )
                    )
                );
                replaceTokens(tokenTree, matchLocation, matchEnd, newTokenPtr);
                currTreeStr = Token::tokenListAsTypeStr(tokenTree);
            }
        }
        
        if(currTreeStr == oldStr) {
            throwUnexpectedTokenException(currTreeStr, tokenTree);
        }
    }
    
    return CompoundToken(CompoundTokenType::Program, tokenTree);
}
