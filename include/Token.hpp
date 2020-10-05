#pragma once

#include <string>
#include <vector>
#include <utility>

/*
 * These are more elegant in haskell
 * Consider:
 * 
 * data SymbolTokenType = Keyword | Boolean | IntType | Float | ... | NoSymbol
 * data CompoundTokenType = RawType | Tuple | List | ... | NoCompound
 * data Token =
 *      SymbolToken SymbolTokenType String (Int, Int)
 *      | CompoundToken CompoundTokenType [Token]
 * 
 * I've chosen to use Inheritance here because this shouldn't really change,
 * it matches the UML, and it works
 */ 
namespace kisslang {
    
    struct Token {};
    
    enum class SymbolTokenType {
        Keyword = 'k',      Boolean = 'b',      Integer = 'i',
        Float = 'f',        Character = 'c',    String = '\'',
        TypeChar = '@',     Identifier = 'n',   Parenth = '(', 
        Bracket = '[',      Brace = '{',        ReturnOp = '>',
        DoubleArrow = '<',  Operator = '=',     TypeOp = ':',
        MemberOp = '.',
        
        None = '?'
    };
    
    struct SymbolToken : public Token {
        const SymbolTokenType type;
        const std::string value;
        const std::pair<int, int> position;
        
        SymbolToken(
            const SymbolTokenType &type, const std::string &value,
            const std::pair<int, int> &position
        );
    };
    
    enum class CompoundTokenType {
        RawType = 'r',      Tuple = ',',        List = 'l',
        Struct = 's',       StructAccess = 'S', Type = 't',
        TypeName = 'N',     Body = '}',         FuncDef = 'F',
        Loop = 'L',         Cast = 'a',         StructDef = 'd',
        Statement = 'I',    Program = 'p',
        
        None = '!'
    };
    
    struct CompoundToken : public Token {
        const CompoundTokenType type;
        const std::vector<Token> children;
        
        CompoundToken(
            const CompoundTokenType &type, const std::vector<Token> &children
        );
    };
}
