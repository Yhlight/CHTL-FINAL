#include "Arg.h"
#include <stdexcept>

void Arg::add(const AtomArg& arg) {
    args.push_back(arg);
}

void Arg::bind(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    for (auto& arg : args) {
        if (arg.placeholder == placeholder) {
            arg.bind(func);
        }
    }
}

std::string Arg::match(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    int& count = match_counts[placeholder];
    int current_match = 0;
    for (auto& arg : args) {
        if (arg.placeholder == placeholder) {
            if (current_match == count) {
                count++;
                return func(arg.value);
            }
            current_match++;
        }
    }
    throw std::runtime_error("No matching argument found for placeholder: " + placeholder);
}

void Arg::fillValue(const Arg& result) {
    if (args.size() != result.args.size()) {
        // A more robust implementation would handle this mismatch.
        // For now, we fill what we can.
    }
    for (size_t i = 0; i < args.size() && i < result.args.size(); ++i) {
        // The value from the scanner is in the 'placeholder' field of the result's AtomArg
        // because of how AtomArg is constructed.
        args[i].fillValue(result.args[i].placeholder);
    }
}

void Arg::transform(const std::string& format) {
    transformed_output = format;
}
