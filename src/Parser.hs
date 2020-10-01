module Parser(lexTokens, parseAst) where

import TokenData(Token(..), CompoundTokenType(..))

lexTokens :: String -> [Token]
lexTokens code =
    []

parseAst :: [Token] -> Token
parseAst tokens =
    CompoundToken Program []
