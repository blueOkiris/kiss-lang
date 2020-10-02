#pragma once

#include <string>
#include <any>
#include <exception>

namespace kisslang {
    struct LocalStackUnderflow : public std::exception {
        LocalStackUnderflow(
            const std::string localName, const std::string argString
        );
        const char *what() const throw();
        
        private:
            const std::string _localName;
            const std::string _argString;
    };

    struct ConstDoesNotExist : public std::exception {
        ConstDoesNotExist(
            const std::string localName, const std::string argString,
            const std::string identifier
        );
        const char *what() const throw();
        
        private:
            const std::string _localName;
            const std::string _argString;
            const std::string _identifier;
    };

    struct ConstAlreadyExist : public std::exception {
        ConstAlreadyExist(
            const std::string localName, const std::string argString,
            const std::string identifier
        );
        const char *what() const throw();
        
        private:
            const std::string _localName;
            const std::string _argString;
            const std::string _identifier;
    };
}
