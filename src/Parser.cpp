#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <Error.hpp>
#include <Token.hpp>
#include <ParserHelpers.hpp>
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
    // Generate starting list
    std::vector<std::shared_ptr<Token>> tokenTree;
    copySymbolTokensToParentPtrs(tokenTree, tokens);
    
    auto currTreeStr = Token::tokenListAsTypeStr(tokenTree);
    while(nonStatementIndex(currTreeStr) != -1) {
        const auto oldStr = currTreeStr;
        
        std::string matchStr;
        for(const auto &regexTokenPair : compoundRegexs) {      
            while(matchRegex(regexTokenPair.first, currTreeStr, matchStr)) {
                const auto matchStart = currTreeStr.find(matchStr);
                const auto matchEnd = matchStart + matchStr.length();
                const auto newTokenPtr = std::make_shared<CompoundToken>(
                    CompoundToken(
                        regexTokenPair.second,
                        std::vector<std::shared_ptr<Token>>(
                            tokenTree.begin() + matchStart,
                            tokenTree.begin() + matchEnd
                        )
                    )
                );
                replaceTokens(tokenTree, matchStart, matchEnd, newTokenPtr);
                currTreeStr = Token::tokenListAsTypeStr(tokenTree);
            }
        }
        
        if(currTreeStr == oldStr) {
            throwUnexpectedTokenException(currTreeStr, tokenTree);
        }
    }
    
    return CompoundToken(CompoundTokenType::Program, tokenTree);
}
