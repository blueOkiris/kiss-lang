module Compiler(generateCode, compile) where

import Control.Exception(catch, throwIO)
import System.IO.Error(isDoesNotExistError)
import System.Directory(removeFile, createDirectoryIfMissing)
import System.FilePath.Posix (takeDirectory)
import TokenData(CompoundToken(..))

generateCode :: CompoundToken -> String
generateCode program =
    ""

createAndWriteFile :: FilePath -> String -> IO ()
createAndWriteFile path content = do
    createDirectoryIfMissing True $ takeDirectory path
    writeFile path content

removeIfExists :: FilePath -> IO ()
removeIfExists fileName =
    removeFile fileName `catch` handleExists
    where handleExists e
            | isDoesNotExistError e = return ()
            | otherwise = throwIO e

compile :: String -> String -> IO()
compile outputFileName code = do
    createAndWriteFile outputFileName code
    removeIfExists outputFileName
