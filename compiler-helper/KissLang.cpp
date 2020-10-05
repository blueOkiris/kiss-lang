#include <utility>
#include <map>
#include <any>
#include <vector>
#include <string>
#include <KissError.hpp>
#include <KissLang.hpp>

using namespace kisslang;

std::vector<StackFrame> kisslang::highestScopeStack;

StackFrame::StackFrame(const std::string localName, const std::any _argument) :
        _localStack(), _localConstants(), _localName(localName),
        _input(_argument) {
}

void StackFrame::push(const std::any value) {
    if(value.type() == typeid(std::string(""))) {
        try {
            _localStack.push_back(
                _localConstants.at(std::any_cast<std::string>(newConst));
            );
        } catch(std::out_of_range oor) {
            throw new ConstDoesNotExist(_localName, _argument.type().name());
        }
    } else {
        _localStack.push_back(newConst);
    }
}

const std::any StackFrame::pop() {
    if(_localStack.empty()) {
        throw new LocalStackUnderflow(_localName, _argument.type().name());
    }
    
    if(_localStack.back().type() == typeid(std::string(""))) {
        const auto ident = _localStack.pop_back();
        try {
            return _localConstants.at(ident);
        } catch(std::out_of_range oor) {
            throw new ConstDoesNotExist(
                _localName, _argument.type().name(),
                any_cast<const std::string>(ident)
            );
        }
    }
    return _localStack.pop_back();
}

void StackFrame::addConst(
        std::pair<const std::string, const std::any> newConst) {
    auto [it, result] = _localConstants.emplace(newConst);
    if(!result) {
        throw new ConstAlreadyExist(
            _localName, _argument.type().name(), newConst.first
        );
    }
}
