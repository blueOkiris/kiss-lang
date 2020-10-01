module TokenData(   SymbolTokenType(..), CompoundTokenType(..)
                    , Token(..), SymbolToken(..), CompoundToken(..) ) where

data SymbolTokenType =  Keyword         | Boolean       | IntType
                        | FloatingPoint | Character     | StringLiteral
                        | TypeChar      | Identifier    | Parenth
                        | Bracket       | Brace         | ReturnOp
                        | DoubleArrow   | Operator      | TypeOp
                        | MemberOp

data CompoundTokenType =    RawType         | Tuple         | List
                            | StructAccess  | Type          | TypeName
                            | Body          | FuncDef       | Loop
                            | Cast          | StructDef     | Stmt
                            | Program

data SymbolToken = Symbol SymbolTokenType String (Int, Int)
data CompoundToken = Compound CompoundTokenType [Token]
data Token = SymbolToken SymbolToken | CompoundToken CompoundToken
