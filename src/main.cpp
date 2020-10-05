#include <Parser.hpp>
#include <Compiler.hpp>

using namespace kisslang;

int main(int argc, char **args) {
    const auto tokens = Parser::lexTokens("");
    const auto ast = Parser::parseAst(tokens);
    return 0;
}
