#pragma once

#include <string>
#include <vector>
#include <memory>
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
    
    struct Token {
        virtual const bool isSymbol() const = 0;
        virtual const std::string str() const = 0;
        static const std::string tokenListAsTypeStr(
            const std::vector<std::shared_ptr<Token>> &tokens
        );
    };
    
    enum class SymbolTokenType {
        Keyword = 'k',      Boolean = 'b',      Integer = 'i',
        Float = 'f',        Character = 'c',    String = '\'',
        TypeChar = '@',     Identifier = 'n',   Parenth = '(', 
        Bracket = '[',      Brace = '{',        ReturnOp = '>',
        DoubleArrow = '<',  Operator = '=',     TypeOp = ':',
        MemberOp = '.',     Dollar = '$',
        
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
        const bool isSymbol() const override;
        const std::string str() const override;
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
        const std::vector<std::shared_ptr<Token>> children;
        
        CompoundToken(
            const CompoundTokenType &newType,
            const std::vector<std::shared_ptr<Token>> &tokens
        );
        const bool isSymbol() const override;
        const std::string str() const override;
    };
}
