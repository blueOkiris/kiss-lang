#pragma once

#include <vector>
#include <string>
#include <utility>
#include <map>
#include <any>

namespace kisslang {
    class StackFrame {
        private:
            std::vector<const std::any> _localStack;
            std::map<const std::string, const std::any> _localConstants;
            const std::string _localName;
            const std::any _argument;
        
        public:
            StackFrame(const std::string localName, const std::any _argument);
            
            void addConst(
                std::pair<const std::string, const std::any> newConst
            );
            void push(const std::any value);
            const std::any pop();
    };
    
    extern std::vector<any> highestScopeStack;
}
