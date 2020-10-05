#include <exception>
#include <any>
#include <string>

using namespace kisslang;

LocalStackOverflow::LocalStackUnderflow(
        const std::string localName, const std::string argString) :
        _localName(localName), _argString(argString) {
}

const char *LocalStackOverflow::what() const throw() {
    return
        "Error: Not enough items on stack!\n"
        "Location: Function " _localName
        " called with argument " _argString "\n";
}

ConstDoesNotExist::ConstDoesNotExist(
        const std::string localName, const std::string argString,
        const std::string identifier) :
        _localName(localName), _argString(argString), _identifier(identifier) {
}

const char *ConstDoesNotExist::what() const throw() {
    return
        "Error: Constant '" _identifier "' has not been declared!\n"
        "Location: Function " _localName
        " called with argument " _argString "\n";
}

ConstAlreadyExist::ConstAlreadyExist(
        const std::string localName, const std::string argString,
        const std::string identifier) :
        _localName(localName), _argString(argString), _identifier(identifier) {
}

const char *ConstAlreadyExist::what() const throw() {
    return
        "Error: Constant '" _identifier "' has already been declared!\n"
        "Location: Function " _localName
        " called with argument " _argString "\n";
}

