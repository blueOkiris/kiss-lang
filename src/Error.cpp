#include <cstring>
#include <string>
#include <Error.hpp>

using namespace kisslang;

SmoochException::SmoochException(std::string message, int line, int col) :
        _message(message), _line(line), _col(col) {
    const auto exceptionStr =
        "Smooch Exception '" + message + "' at line " + std::to_string(line)
        + ", col " + std::to_string(col);
    const char *exceptionCStr = exceptionStr.c_str();
    
    _whatStr = new char[exceptionStr.length() + 1];
    strcpy(_whatStr, exceptionCStr);
}

SmoochException::~SmoochException() {
    delete _whatStr;
}

const char *SmoochException::what() const throw() {
    return _whatStr;
}

UnknownTokenException::UnknownTokenException(int line, int col) :
        SmoochException("Unknown token", line, col) {
}

UnexpectedTokenException::UnexpectedTokenException(std::string tokenStr) :
        SmoochException(
            "Unexpected token (see token str \"" + tokenStr + "\")", 0, 0
        ) {
}
