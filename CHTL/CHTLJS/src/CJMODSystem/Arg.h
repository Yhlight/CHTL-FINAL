#ifndef ARG_H
#define ARG_H

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <map>

class AtomArg {
public:
    std::string placeholder;
    std::string value;
    std::function<std::string(const std::string&)> bind_func;

    AtomArg(const std::string& ph) : placeholder(ph) {}

    void bind(std::function<std::string(const std::string&)> func) {
        bind_func = func;
    }

    void fillValue(const std::string& val) {
        value = val;
    }
};

class Arg {
public:
    std::vector<AtomArg> args;
    std::map<std::string, int> match_counts;
    std::string transformed_output;

    void add(const AtomArg& arg) {
        args.push_back(arg);
    }

    void print() const {
        std::cout << "[";
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << "\"" << args[i].placeholder << "\"";
            if (i < args.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }

    AtomArg& operator[](size_t index) {
        return args[index];
    }

    const AtomArg& operator[](size_t index) const {
        return args[index];
    }

    // Other methods to be implemented in Arg.cpp
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    std::string match(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    void fillValue(const Arg& result);
    void transform(const std::string& format);
};

#endif // ARG_H
