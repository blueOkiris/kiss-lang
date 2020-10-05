#include <string>
#include <vector>
#include <regex>
#include <map>
#include <Token.hpp>
#include <Parser.hpp>

using namespace kisslang;

static const std::map<std::string, SymbolTokenType> symbolRegexs = {
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

static const std::map<std::string, CompoundTokenType> compoundRegexs = {
    { "[bic'nf]",                   CompoundTokenType::RawType },
    { "\\(tt\\(",                   CompoundTokenType::Tuple },
    { "\\[t+\\[",                   CompoundTokenType::List },
    { "\\(n\\(\\{t*\\{",            CompoundTokenType::Struct },
    { "n(\\.n)+",                   CompoundTokenType::StructAccess },
    { "[r,lsS]",                    CompoundTokenType::TypeName },
    { "\\{I*\\{",                   CompoundTokenType::Body },
    { "kn:N>N",                     CompoundTokenType::FuncDef },
    { "k\\}",                       CompoundTokenType::Loop },
    { "<N<",                        CompoundTokenType::Cast },
    { "k\\{(n:N)*\\{",              CompoundTokenType::StructDef },
    { "[tFLa=d]",                   CompoundTokenType::Statement }
};

const std::vector<SymbolToken> Parser::lexTokens(const std::string &code) {
    return std::vector<SymbolToken>();
}

const CompoundToken Parser::parseAst(const std::vector<SymbolToken> &tokens) {
    return CompoundToken(
        CompoundTokenType::None, std::vector<Token>()
    );
}
