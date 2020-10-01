module Compiler(generateCode, compile) where

import TokenData(CompoundToken(..))
import System.Directory (createDirectoryIfMissing)
import System.FilePath.Posix (takeDirectory)

generateCode :: CompoundToken -> String
generateCode program =
    ""

createAndWriteFile :: FilePath -> String -> IO ()
createAndWriteFile path content = do
    createDirectoryIfMissing True $ takeDirectory path
    writeFile path content

compile :: String -> String -> IO()
compile outputFileName code = do
    createAndWriteFile outputFileName code
