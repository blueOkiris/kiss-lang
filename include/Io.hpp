#pragma once

#include <string>

/*
 * This is a set of helper functions for the main function
 * Which handle IO stuff like handling arguments and reading files
 */
namespace kisslang {
    namespace Io {
        const int sourceCodeFromArgs(
            std::string &code, std::string &moduleName,
            int argc, char **args
        );
    }
}
