module Main where

import System.Environment(getArgs)
import System.Directory(doesFileExist, doesDirectoryExist)

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
        
        if fileExists || (moduleExists && mainFileExists) then do
            let fileName =  if fileExists then (args !! 0)
                            else if moduleExists then (args !! 0) ++ "/main.kiss"
                            else ""
            codeStr <- readFile fileName
            
            putStrLn("Done.")
        else
            putStrLn "No module of filename given."
    else
        putStrLn "No module or filename given."
