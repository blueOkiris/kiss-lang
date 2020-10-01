module Main where

import System.Environment(getArgs)
import System.Directory(doesFileExist, doesDirectoryExist)
import Data.Char(toUpper, toLower)
import Data.String.Utils(replace)
import Parser(lexTokens, parseAst)
import Compiler(generateCode, compile)
import System.Console.ParseArgs(baseName)

-- Primary function for compiler
compileSource :: String -> Bool -> Bool -> IO()
compileSource moduleName fileExists moduleExists = do
    let fileName =  if fileExists then moduleName
                            else if moduleExists then
                                moduleName++ "/main.kiss"
                            else ""
    codeStr <- readFile fileName
    let tokens = lexTokens codeStr
    let ast = parseAst tokens
    let cppCode = generateCode ast
    let cppFileName = getCppFileName $ baseName moduleName
    compile cppFileName cppCode

capitalize :: String -> String
capitalize (head:tail) = toUpper head : map toLower tail
capitalize [] = []

getCppFileName :: String -> String
getCppFileName moduleName =
    capitalModName ++ ".cpp"
    where
        baseName = replace ".kiss" "" moduleName
        capitalModName = capitalize baseName

-- Read in arguments and call compile source if eveything's in order
main :: IO ()
main = do
    args <- getArgs
    if length args == 1 then do
        fileExists <- doesFileExist (args !! 0)
        moduleExists <- doesDirectoryExist (args !! 0)
        mainFileExists <- doesFileExist $ (args !! 0) ++ "/main.kiss"
        
        if fileExists then
            putStrLn $ "Compiling file '" ++ (args !! 0) ++ "'."
        else if moduleExists && mainFileExists then
            putStrLn $ "Compiling module '" ++ (args !! 0) ++ "'."
        else if moduleExists && (not mainFileExists) then
            putStrLn $
                "Could not find main file in module '" ++ (args !! 0) ++ "'."
        else
            putStrLn $ "No such file or module '" ++ (args !! 0) ++ "'"
        
        if fileExists || (moduleExists && mainFileExists) then
            compileSource (args !! 0) fileExists moduleExists
        else
            putStrLn "No module of filename given."
    else
        putStrLn "No module or filename given."
