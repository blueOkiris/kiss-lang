module Parser(lexTokens, parseAst) where

import Debug.Trace(trace)
import Data.Maybe(fromJust, isJust, Maybe(..))
import Data.List(isPrefixOf)
import Data.String.Utils(replace)
import Data.Char(isSpace)
import Text.RegexPR(matchRegexPR)
import TokenData(   Token(..), CompoundTokenType(..), SymbolTokenType(..)
                    , SymbolToken(..), CompoundToken(..)
                    , symbolTokenData )

-- LEXER SECTION

-- Set of all symbol regexs. Must be in the right order (see below)
lexerRegexs :: [(SymbolTokenType, String)]
lexerRegexs =
    [ (Keyword,         "loop|func|struct")
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

-- END LEXER

-- PARSER THIRD LEVEL

-- <brace> { <stmt> } <brace>
parseBody :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseBody tokens index = (Nothing, index)

{- <type-name> := <type-char> | <identifier>
 -      | <parenth> <type-name> <type-name> <parenth>
 -      | <bracket> <type-name> <bracket> -}
parseTypeName :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseTypeName tokens index = (Nothing, index)
    
{- <raw-type> := <boolean> | <integer> | <character> | <string> | <identifier>
 -      | <float> -}
parseRawType :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseRawType tokens index = (Nothing, index)

-- <tuple> := <parenth> <type> <type> <parenth>
parseTuple :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseTuple tokens index = (Nothing, index)

-- <list> := <bracket> [ <type> ] <bracket>
parseList :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseList tokens index = (Nothing, index)

-- <struct-access> := <identifier> [ <member-op> <identifier> ]
parseStructAccess :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseStructAccess tokens index = (Nothing, index)

-- <struct> := <parenth> <identifier> <parenth> <brace> { <type> } <brace>
parseStruct :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseStruct tokens index = (Nothing, index)
    
parseIdentTypeNamePairs :: [SymbolToken] -> Int -> (Maybe [Token], Int)
parseIdentTypeNamePairs tokens index = (Nothing, index)

-- PARSER SECOND LEVEL

-- <type> := <raw-type> | <tuple> | <list> | <struct-access> | <struct>
parseType :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseType tokens index
    | isJust rawTypeMaybe =
        ( Just (Compound Type [ CompoundToken $ fromJust rawTypeMaybe ])
        , rawTypeIndex + 1 )
    | isJust tupleMaybe =
        ( Just (Compound Type [ CompoundToken $ fromJust tupleMaybe ])
        , tupleIndex + 1 )
    | isJust listMaybe =
        ( Just (Compound Type [ CompoundToken $ fromJust listMaybe ])
        , listIndex + 1 )
    | isJust structAccessMaybe =
        ( Just (Compound Type [ CompoundToken $ fromJust structAccessMaybe ])
        , structAccessIndex + 1 )
    | isJust structMaybe =
        ( Just (Compound Type [ CompoundToken $ fromJust structMaybe ])
        , structIndex + 1 )
    | otherwise = (Nothing, index)
    where
        (rawTypeMaybe, rawTypeIndex) = parseRawType tokens index
        (tupleMaybe, tupleIndex) = parseTuple tokens index
        (listMaybe, listIndex) = parseList tokens index
        (structAccessMaybe, structAccessIndex) = parseStructAccess tokens index
        (structMaybe, structIndex) = parseStruct tokens index

{- <func-def> := <keyword> <identifier>
 -      <type-op> <type-name> <ret-op> <type-name> <body> -}
parseFuncDef :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseFuncDef tokens index
    | (isJust bodyMaybe) && (isJust typeName2Maybe) && (isJust typeName1Maybe)
        && (keywordTokenType == Keyword) && (keywordStr == "func")
        && (identTokenType == Identifier) && (typeOpType == TypeOp)
        && (retOpType == ReturnOp) =
            ( Just $ Compound FuncDef
                [ SymbolToken $ tokens !! index
                , SymbolToken $ tokens !! (index + 1)
                , SymbolToken $ tokens !! (index + 3)
                , CompoundToken $ fromJust typeName1Maybe
                , SymbolToken $ tokens !! (index + typeName1Index + 1)
                , CompoundToken $ fromJust typeName2Maybe 
                , CompoundToken $ fromJust bodyMaybe ]
            , bodyIndex + 1 )
    | otherwise = (Nothing, index)
    where
        (keywordTokenType, (keywordStr, keywordPos)) =
            if index < (length tokens) then
                symbolTokenData (tokens !! index)
            else
                (NoSymbol, ("", (1, 1)))
        (identTokenType, (identStr, identPos)) =
            if (index + 1) < (length tokens) then
                symbolTokenData (tokens !! (index + 1))
            else
                (NoSymbol, ("", (1, 1)))
        (typeOpType, (typeOpStr, typeOpPos)) =
            if (index + 2) < (length tokens) then
                symbolTokenData (tokens !! (index + 2))
            else
                (NoSymbol, ("", (1, 1)))
        (typeName1Maybe, typeName1Index) = parseTypeName tokens (index + 3)
        (retOpType, (retOpStr, retOpPos)) =
            if (index + 2) < (length tokens) then
                symbolTokenData (tokens !! (typeName1Index + 1))
            else
                (NoSymbol, ("", (1, 1)))
        (typeName2Maybe, typeName2Index) =
            parseTypeName tokens (typeName1Index + 1)
        (bodyMaybe, bodyIndex) = parseBody tokens (typeName2Index + 1)

-- <loop> := <keyword> <body>
parseLoop :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseLoop tokens index
    | (tokenType == Keyword) && (tokenStr == "loop") && (isJust body) =
        ( Just $ Compound Loop 
            [ SymbolToken $ tokens !! index, CompoundToken $ fromJust body ]
        , bodyIndex + 1 )
    | otherwise = (Nothing, index)
    where
        (tokenType, (tokenStr, position)) = 
            if index < (length tokens) then
                symbolTokenData (tokens !! index)
            else
                (NoSymbol, ("", (1, 1)))
        (body, bodyIndex) = parseBody tokens (index + 1)

-- <cast> := <double-arrow> <type-name> <double-arrow>
parseCast :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseCast tokens index
    | (isJust typeNameMaybe)
        && (token1Type == DoubleArrow) && (token2Type == DoubleArrow) =
            ( Just $ Compound Cast
                [ SymbolToken $ tokens !! index
                , CompoundToken $ fromJust typeNameMaybe
                , SymbolToken $ tokens !! (typeNameIndex + 1) ]
            , typeNameIndex + 2 )
    | otherwise = (Nothing, index)
    where
        (token1Type, (token1Str, position1)) = 
            if (index < (length tokens)) then
                symbolTokenData (tokens !! index)
            else
                (NoSymbol, ("", (1, 1)))
        (typeNameMaybe, typeNameIndex) = parseTypeName tokens (index + 1)
        (token2Type, (token2Str, position2)) = 
            if ((typeNameIndex + 1) < (length tokens)) then
                symbolTokenData (tokens !! (typeNameIndex + 1))
            else
                (NoSymbol, ("", (1, 1)))

-- <operator> := /\+|-|\/|%|\+\+|--|==|!=|>|<|>=|<=|&&|\|\||!|&|\^|~|=/
parseOperator :: [SymbolToken] -> Int -> ((Maybe SymbolToken), Int)
parseOperator tokens index
    | tokenType == Operator =
        (Just $ tokens !! index, index + 1)
    | otherwise = (Nothing, index)
    where
        (tokenType, (tokenStr, position)) = 
            if (index < (length tokens)) then
                symbolTokenData (tokens !! index)
            else
                (NoSymbol, ("", (1, 1)))

{- <struct-def> := <keyword> <brace>
 -      { <identifier> <type-op> <type-name> } <brace> -}
parseStructDef :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseStructDef tokens index
    | (keywordTokenType == Keyword) && (keywordStr == "struct")
        && (brace1Type == Brace) && (brace1Str == "{")
        && (isJust identTypeNamePairs)
        && (brace2Type == Brace) && (brace2Str == "}") =
            ( Just $ Compound StructDef
                ([ SymbolToken $ tokens !! index
                , SymbolToken $ tokens !! (index + 1) ] ++ 
                    fromJust identTypeNamePairs ++
                        [ SymbolToken $ tokens !! (identTypeNameIndex + 1) ])
            , identTypeNameIndex + 2 )
    | otherwise = (Nothing, index)
    where
        (keywordTokenType, (keywordStr, keywordPos)) =
            if index < (length tokens) then
                symbolTokenData (tokens !! index)
            else
                (NoSymbol, ("", (1, 1)))
        (brace1Type, (brace1Str, brace1Pos)) =
            if (index + 1) < (length tokens) then
                symbolTokenData (tokens !! (index + 1))
            else
                (NoSymbol, ("", (1, 1)))
        (identTypeNamePairs, identTypeNameIndex) =
            parseIdentTypeNamePairs tokens (index + 2)
        (brace2Type, (brace2Str, brace2Pos)) =
            if (identTypeNameIndex + 1) < (length tokens) then
                symbolTokenData (tokens !! (identTypeNameIndex + 1))
            else
                (NoSymbol, ("", (1, 1)))

-- PARSER TOP LEVEL

-- <stmt> := <type> | <func-def> | <loop> | <cast> | <operator> | <struct-def>
parseStmt :: [SymbolToken] -> Int -> ((Maybe CompoundToken), Int)
parseStmt tokens index
    | isJust typeMaybe =
        ( Just (Compound Stmt [ CompoundToken $ fromJust typeMaybe ])
        , typeNewIndex )
    | isJust funcDefMaybe =
        ( Just (Compound Stmt [ CompoundToken $ fromJust funcDefMaybe ])
        , funcDefNewIndex)
    | isJust loopMaybe =
        ( Just (Compound Stmt [ CompoundToken $ fromJust loopMaybe ])
        , loopNewIndex)
    | isJust castMaybe =
        ( Just (Compound Stmt [ CompoundToken $ fromJust castMaybe ])
        , castNewIndex)
    | isJust operatorMaybe =
        ( Just (Compound Stmt [ SymbolToken $ fromJust operatorMaybe ])
        , operatorNewIndex)
    | isJust structDefMaybe =
        ( Just (Compound Stmt [ CompoundToken $ fromJust structDefMaybe ])
        , structDefNewIndex)
    | otherwise = (Nothing, index + 1)
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
    | isJust stmtMaybe =  recur (current ++ [stmt])
    | otherwise = recur current
    where
        (stmtMaybe, newIndex) = parseStmt tokens index
        stmt =  if isJust stmtMaybe then
                    CompoundToken $ fromJust stmtMaybe
                else 
                    CompoundToken $ Compound Stmt []
        recur = parseProgram tokens newIndex

parseAst :: [SymbolToken] -> CompoundToken
parseAst tokens =
    Compound Program $ parseProgram tokens 0 []
