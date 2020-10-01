module Parser(lexTokens, parseAst) where

import TokenData(   Token(..), CompoundTokenType(..)
                    , SymbolToken(..), CompoundToken(..) )

lexTokens :: String -> [SymbolToken]
lexTokens code =
    []

parseAst :: [SymbolToken] -> CompoundToken
parseAst tokens =
    Compound Program []
