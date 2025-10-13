#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

    class Node {
    public:
        virtual ~Node() = default;
        virtual std::string ToString() const = 0;
    };

    class Statement : public Node {
    public:
        // Placeholder for now
    };

    class Expression : public Node {
    public:
        // Placeholder for now
    };

    class Program : public Node {
    public:
        std::vector<std::unique_ptr<Statement>> statements;
        std::string ToString() const override {
            std::string out;
            for (const auto& stmt : statements) {
                out += stmt->ToString();
            }
            return out;
        }
    };

} // namespace CHTL
