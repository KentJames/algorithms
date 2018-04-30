import Text.Regex.PCRE
import qualified Data.ByteString.Lazy.Char8 as L
import System.Environment
import System.IO


regex_file :: L.ByteString -> Regex -> Bool
regex_file input regex = matchTest regex input


argval :: Read p => [String] -> Int -> p -> p
argval args n def = if length args >  n then
                      read (args !! n)
                    else def  

defregex :: String
defregex = "^(.*?(\bLorem\b)[^$]*)" 

boolToString :: Bool -> String
boolToString True = "TRUE"
boolToString False = "FALSE"


main :: IO()
main = do
  args <- getArgs
  let regex = argval args 1 defregex
  let compregex = makeRegex regex :: Regex
  content <- readFile(args !! 0)
  print args
  let regex_cont = regex_file (L.pack content) compregex
  print (boolToString regex_cont)



