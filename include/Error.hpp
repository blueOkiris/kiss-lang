#pragma once

#include <string>
#include <exception>

namespace kisslang {
    struct SmoochException : public std::exception {
        SmoochException(std::string message, int line, int col);
        ~SmoochException();
        const char *what() const throw();
        protected:
            std::string _message;
            int _line, _col;
            char *_whatStr;
    };
    
    struct UnknownTokenException : public SmoochException {
        UnknownTokenException(int line, int col);
    };
    
    struct UnexpectedTokenException : public SmoochException {
        UnexpectedTokenException(std::string tokenStr);
    };
}
