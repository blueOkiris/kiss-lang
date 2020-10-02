module Parser(lexTokens, parseAst) where

import Debug.Trace(trace)
import Data.Maybe(fromJust, isJust, Maybe(..))
import Data.List(isPrefixOf)
import Data.String.Utils(replace)
import Data.Char(isSpace)
import Text.RegexPR(matchRegexPR)
import TokenData(   Token(..), CompoundTokenType(..), SymbolTokenType(..)
                    , SymbolToken(..), CompoundToken(..) )

-- Set of all symbol regexs. Must be in the right order (see below)
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

{- Find the first regex (in order) that applies to the START of the string
 - This can only work if the regexs don't overlap.
 - So basically don't call /\?/ before /\?\?/
 - because otherwise you'll lex [ ?, ? ] instead of [ ?? ] -}
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

{- Take the current string location, call the matching function,
 - and return the new token as well as how far to move in code
 - to pass the found token -}
lexToken :: String -> Int -> (Int, Int) -> (SymbolToken, Int)
lexToken code index position =
    (Symbol tokenType matchStr position, index + (length matchStr))
    where
        (tokenType, matchStr) = getFirstMatch (drop index code) 0

-- Move through the code, parsing tokens and skipping whitespace
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

-- <type> := <raw-type> | <tuple> | <list> | <struct-access> | <struct>
parseType :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseType tokens index =
    (Just (Compound Type []), index + 1)

{- <func-def> :=
 -      <keyword> <identifier>
 -      <type-op> <type-name> <ret-op> <type-name> <body> -}
parseFuncDef :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseFuncDef tokens index =
    (Just (Compound FuncDef []), index + 1)

-- <loop> := <keyword> <body>
parseLoop :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseLoop tokens index =
    (Just (Compound Loop []), index + 1)

-- <cast> := <double-arrow> <type-name> <double-arrow>
parseCast :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseCast tokens index =
    (Just (Compound Cast []), index + 1)

-- <operator> := /\+|-|\/|%|\+\+|--|==|!=|>|<|>=|<=|&&|\|\||!|&|\^|~|=/
parseOperator :: [SymbolToken] -> Int -> ((Maybe SymbolToken), Int)
parseOperator tokens index =
    (Just $ tokens !! index, index + 1)

{- <struct-def> := 
 -      <keyword> <brace> { <identifier> <type-op> <type-name> } <brace> -}
parseStructDef :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseStructDef tokens index =
    (Just (Compound StructDef []), index + 1)

-- <stmt> := <type> | <func-def> | <loop> | <cast> | <operator> | <struct-def>
parseStmt :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseStmt tokens index
    | isJust typeMaybe =
        (Just (Compound Stmt [ CompoundToken $ fromJust typeMaybe ]),
            typeNewIndex)
    | isJust funcDefMaybe =
        (Just (Compound Stmt [ CompoundToken $ fromJust funcDefMaybe ]),
            funcDefNewIndex)
    | isJust loopMaybe =
        (Just (Compound Stmt [ CompoundToken $ fromJust loopMaybe ]),
            loopNewIndex)
    | isJust castMaybe =
        (Just (Compound Stmt [ CompoundToken $ fromJust castMaybe ]),
            castNewIndex)
    | isJust operatorMaybe =
        (Just (Compound Stmt [ SymbolToken $ fromJust operatorMaybe ]),
            operatorNewIndex)
    | isJust structDefMaybe =
        (Just (Compound Stmt [ CompoundToken $ fromJust structDefMaybe ]),
            structDefNewIndex)
    | otherwise = (Just (Compound Stmt []), index + 1)
    where
        (typeMaybe, typeNewIndex) = parseType tokens index
        (funcDefMaybe, funcDefNewIndex) = parseFuncDef tokens index
        (loopMaybe, loopNewIndex) = parseLoop tokens index
        (castMaybe, castNewIndex) = parseCast tokens index
        (operatorMaybe, operatorNewIndex) = parseOperator tokens index
        (structDefMaybe, structDefNewIndex) = parseStructDef tokens index

-- <program> := [ <stmt> ]
parseProgram :: [SymbolToken] -> Int -> [Token] -> [Token]
parseProgram tokens index current
    | index >= length tokens = []
    | isJust stmtMaybe =  parseProgram tokens newIndex (current ++ [stmt])
    | otherwise =  parseProgram tokens (index + 1) current
    where
        (stmtMaybe, newIndex) = parseStmt tokens index
        stmt =  if isJust stmtMaybe then
                    CompoundToken $ fromJust stmtMaybe
                else 
                    CompoundToken $ Compound Stmt []

parseAst :: [SymbolToken] -> CompoundToken
parseAst tokens =
    Compound Program $ parseProgram tokens 0 []
