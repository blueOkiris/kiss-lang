module Parser(lexTokens, parseAst) where

import Debug.Trace(trace)
import Data.Maybe(fromJust, isJust)
import Data.List(isPrefixOf)
import Data.String.Utils(replace)
import Data.Char(isSpace)
import Text.RegexPR(matchRegexPR)
import TokenData(   Token(..), CompoundTokenType(..), SymbolTokenType(..)
                    , SymbolToken(..), CompoundToken(..) )

lexerRegexs :: [(SymbolTokenType, String)]
lexerRegexs =
    [ (Keyword,         "loop|func|import|struct")
    , (Boolean,         "true|false")
    , (IntType,         "([0-9]+|0x[0-9A-Za-z]+|0b[01]+):[1248]")
    , (FloatingPoint,   "([0-9]+\\.[0-9]*|\\.[0-9]+):[48]")
    , (Character,       "0c(\\\\.|[^\\\\])")
    , (StringLiteral,   "'(\\\\.|[^\\\\'])*'")
    , (TypeChar,        "#:[1248]|\\.:[48]|@|\\?")
    , (Identifier,      "[A-Za-z_][A-Za-z_0-9]*")
    , (Parenth,         "\\(|\\)")
    , (Bracket,         "\\[\\]")
    , (Brace,           "\\{\\}")
    , (ReturnOp,        "->")
    , (DoubleArrow,     "<<|>>")
    , (Operator,
        "\\+|-|\\/|%|\\+\\+|--|==|!=|>|<|>=|<=|&&|\\|\\||!|&|\\^|~|=")
    , (TypeOp,          "::")
    , (MemberOp,        "\\.") ]

getFirstMatch :: String -> Int -> (SymbolTokenType, String)
getFirstMatch code matchIndex
    | matchIndex >= length lexerRegexs = (NoSymbol, "")
    | (isJust match) && (isPrefixOf matchStr code) && (matchStr /= "") = 
        (tokenType, matchStr)
    | otherwise = getFirstMatch code (matchIndex + 1)
    where
        (tokenType, regex) = lexerRegexs !! matchIndex
        match = matchRegexPR regex code 
        matchStr =  if (isJust match) then fst $ fst $ fromJust match else ""

lexToken :: String -> Int -> (Int, Int) -> (SymbolToken, Int)
lexToken code index position =
    (Symbol tokenType matchStr position, index + (length matchStr))
    where
        (tokenType, matchStr) = getFirstMatch (drop index code) 0

lexTokensCore :: [SymbolToken] -> String -> Int -> (Int, Int) -> [SymbolToken]
lexTokensCore current code index (line, col)
    | (index < (length code)) && (isSpace currChar) =
        lexTokensCore current code (index + 1) nextPos
    | index < (length code) =
        lexTokensCore (current ++ [nextToken]) code newIndex nextPos
    | otherwise = current
    where
        currChar = if index < (length code) then code !! index else ' '
        (nextToken, newIndex) = if isSpace currChar then
                                    (Symbol NoSymbol "" (0, 0), index + 1)
                                else
                                    lexToken code index (line, col)
        nextPos = if currChar == '\n' then (line + 1, 1) else (line, col + 1)

lexTokens :: String -> [SymbolToken]
lexTokens code =
    lexTokensCore [] code 0 (1, 1)

parseAst :: [SymbolToken] -> CompoundToken
parseAst tokens =
    Compound Program []
