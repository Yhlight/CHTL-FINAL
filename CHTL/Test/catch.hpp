#ifndef CATCH_HPP
#define CATCH_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace Catch {
    struct TestCase {
        std::string name;
        std::function<void()> test_function;
    };

    class TestRegistry {
    public:
        static TestRegistry& instance() {
            static TestRegistry instance;
            return instance;
        }

        void add(const TestCase& test_case) {
            test_cases_.push_back(test_case);
        }

        int run_all_tests() {
            int failures = 0;
            for (const auto& test_case : test_cases_) {
                try {
                    test_case.test_function();
                    std::cout << "[PASSED] " << test_case.name << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "[FAILED] " << test_case.name << ": " << e.what() << std::endl;
                    failures++;
                }
            }
            return failures;
        }

    private:
        TestRegistry() = default;
        std::vector<TestCase> test_cases_;
    };

    #define TEST_CASE(name, description) \
        void test_function_##name(); \
        struct TestRegisterer_##name { \
            TestRegisterer_##name() { \
                Catch::TestRegistry::instance().add({description, test_function_##name}); \
            } \
        }; \
        TestRegisterer_##name registerer_##name; \
        void test_function_##name()

    #define REQUIRE(condition) \
        if (!(condition)) { \
            throw std::runtime_error("Requirement failed: " #condition); \
        }
}

#endif // CATCH_HPP
