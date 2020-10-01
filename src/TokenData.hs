module TokenData(SymbolTokenType(..), CompoundTokenType(..), Token(..)) where

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

data Token =    SymbolToken SymbolTokenType String (Int, Int)
                | CompoundToken CompoundTokenType [ Token ]
