#ifndef ARG_H
#define ARG_H

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <map>

/**
 * @class AtomArg
 * @brief Represents a single atomic argument within a syntax expression.
 *
 * It holds the placeholder, the actual value, and a function for binding transformations.
 */
class AtomArg {
public:
    std::string placeholder;
    std::string value;
    std::function<std::string(const std::string&)> bind_func;

    AtomArg(const std::string& ph) : placeholder(ph) {}

    /**
     * @brief Binds a transformation function to this argument.
     * @param func The function to be bound.
     */
    void bind(std::function<std::string(const std::string&)> func) {
        bind_func = func;
    }

    /**
     * @brief Fills the argument's value.
     * @param val The value to be filled.
     */
    void fillValue(const std::string& val) {
        value = val;
    }
};

/**
 * @class Arg
 * @brief Represents a list of arguments parsed from a syntax expression.
 *
 * This class provides methods for manipulating, matching, and transforming the list of arguments.
 */
class Arg {
public:
    std::vector<AtomArg> args;
    std::map<std::string, int> match_counts;
    std::string transformed_output;

    /**
     * @brief Adds a new atomic argument to the list.
     * @param arg The AtomArg to be added.
     */
    void add(const AtomArg& arg);

    /**
     * @brief Prints the placeholders of the arguments to the console.
     */
    void print() const;

    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;

    /**
     * @brief Binds a transformation function to all arguments with a given placeholder.
     * @param placeholder The placeholder to match (e.g., "$").
     * @param func The transformation function to bind.
     */
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);

    /**
     * @brief Matches the next available argument with a given placeholder and applies a function to it.
     * @param placeholder The placeholder to match.
     * @param func The function to apply to the matched argument's value.
     * @return The result of the function application.
     */
    std::string match(const std::string& placeholder, std::function<std::string(const std::string&)> func);

    /**
     * @brief Fills the values of the arguments from another Arg object.
     * @param result The Arg object containing the values to be filled.
     */
    void fillValue(const Arg& result);

    /**
     * @brief Sets the final transformed output string.
     * @param format The transformed string.
     */
    void transform(const std::string& format);
};

#endif // ARG_H
