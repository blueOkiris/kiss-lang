module TokenData(   SymbolTokenType(..), CompoundTokenType(..)
                    , Token(..), SymbolToken(..), CompoundToken(..) ) where

data SymbolTokenType =  Keyword         | Boolean       | IntType
                        | FloatingPoint | Character     | StringLiteral
                        | TypeChar      | Identifier    | Parenth
                        | Bracket       | Brace         | ReturnOp
                        | DoubleArrow   | Operator      | TypeOp
                        | MemberOp      | NoSymbol
                        deriving(Show)

data CompoundTokenType =    RawType         | Tuple         | List
                            | StructAccess  | Type          | TypeName
                            | Body          | FuncDef       | Loop
                            | Cast          | StructDef     | Stmt
                            | Program       | NoCompound
                            deriving(Show)

data SymbolToken = Symbol SymbolTokenType String (Int, Int) deriving(Show)
data CompoundToken = Compound CompoundTokenType [Token] deriving(Show)
data Token =    SymbolToken SymbolToken 
                | CompoundToken CompoundToken
                deriving(Show)
