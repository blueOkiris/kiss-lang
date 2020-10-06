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
    { "\\{\\}",                             SymbolTokenType::Brace },
    { "->",                                 SymbolTokenType::ReturnOp },
    { "<<|>>",                              SymbolTokenType::DoubleArrow },
    {
        "\\+|-|\\/|%|\\+\\+|--|==|!=|>|<|>=|<=|&&|\\|\\||!|&|\\^|~|=",
        SymbolTokenType::Operator
    },
    { "::",                                 SymbolTokenType::TypeOp },
    { "\\.",                                SymbolTokenType::MemberOp }
};
static const std::vector<std::pair<std::string, CompoundTokenType>> 
        compoundRegexs = {
    { "[bic'nf]",                   CompoundTokenType::RawType },
    { "\\(tt\\(",                   CompoundTokenType::Tuple },
    { "\\[t+\\[",                   CompoundTokenType::List },
    { "\\(n\\(\\{t*\\{",            CompoundTokenType::Struct },
    { "n(\\.n)+",                   CompoundTokenType::StructAccess },
    { "[r,lsS]",                    CompoundTokenType::Type },
    { "[@n]|(\\(NN\\()|(\\[N\\[)",  CompoundTokenType::TypeName },
    { "\\{I*\\{",                   CompoundTokenType::Body },
    { "kn:N>N",                     CompoundTokenType::FuncDef },
    { "k\\}",                       CompoundTokenType::Loop },
    { "<N<",                        CompoundTokenType::Cast },
    { "k\\{(n:N)*\\{",              CompoundTokenType::StructDef },
    { "[tFLa=d]",                   CompoundTokenType::Statement }
};

const std::vector<SymbolToken> Parser::lexTokens(const std::string &code) {
    std::vector<SymbolToken> tokens;
    int line = 1, col = 1;
    for(auto codeIt = code.begin(); codeIt != code.end(); ++codeIt) {
        if(*codeIt == ' ' || *codeIt == '\t' || *codeIt == '\r') {
            continue;
        } else if(*codeIt == '\n') {
            line++;
            col = 1;
            continue;
        }
        
        const auto currStr = std::string(codeIt, code.end());
        bool foundMatches = false;
        for(auto regexTokenPairIt = symbolRegexs.begin();
                regexTokenPairIt != symbolRegexs.end(); ++regexTokenPairIt) {
            std::smatch matches;
            const auto regexStr = regexTokenPairIt->first;
            const std::regex currRegex(regexStr);
            foundMatches = std::regex_search(
                currStr, matches, currRegex
            );
            if(foundMatches) {
                const auto matchStr = matches[0].str();
                
                // If currStr."startswith"(first match string)
                if(currStr.rfind(matchStr, 0) == 0) {
                    tokens.push_back(
                        SymbolToken(
                            regexTokenPairIt->second, matchStr, { line, col }
                        )
                    );
                    codeIt += matchStr.length();
                    col += matchStr.length();
                    break;
                }
            }
        }
        
        if(!foundMatches) {
            throw UnknownTokenException(line, col);
        }
        col++;
    }
    return tokens;
}

inline const bool allStatements(const std::string &str) {
    for(const auto chr : str) {
        if(chr != 'I') {
            return false;
        }
    }
    return true;
}

const CompoundToken Parser::parseAst(const std::vector<SymbolToken> &tokens) {
    std::vector<std::shared_ptr<Token>> tokenTree;
    
    for(auto tokenIt = tokens.begin(); tokenIt != tokens.end(); ++tokenIt) {
        const auto symbolPtr = std::make_shared<SymbolToken>(*tokenIt);
        tokenTree.push_back(std::dynamic_pointer_cast<Token>(symbolPtr));
    }
    
    auto currTreeStr = Token::tokenListAsTypeStr(tokenTree);
    while(!allStatements(currTreeStr)) {
        const auto oldStr = currTreeStr;
        for(auto regexTokenPairIt = compoundRegexs.begin();
                regexTokenPairIt != compoundRegexs.end(); ++regexTokenPairIt) {
            std::smatch matches;
            const auto regexStr = regexTokenPairIt->first;
            const std::regex currRegex(regexStr);
            
            while(std::regex_search(currTreeStr, matches, currRegex)) {
                const auto match = matches[0];
                const auto matchLocation = currTreeStr.find(match.str());
                const auto matchEnd = matchLocation + match.str().length();
                const auto newTokenPtr = std::make_shared<CompoundToken>(
                    CompoundToken(
                        regexTokenPairIt->second,
                        std::vector<std::shared_ptr<Token>>(
                            tokenTree.begin() + matchLocation,
                            tokenTree.begin() + matchEnd
                        )
                    )
                );
                tokenTree.insert(
                    tokenTree.begin() + matchLocation,
                    std::dynamic_pointer_cast<Token>(newTokenPtr)
                );
                tokenTree.erase(
                    tokenTree.begin() + matchLocation + 1,
                    tokenTree.begin() + matchEnd + 1
                );
                currTreeStr = Token::tokenListAsTypeStr(tokenTree);
            }
        }
        if(currTreeStr == oldStr) {
            throw UnexpectedTokenException(currTreeStr);
        }
    }
    return CompoundToken(CompoundTokenType::Program, tokenTree);
}
