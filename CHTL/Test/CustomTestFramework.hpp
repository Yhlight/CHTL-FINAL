#ifndef CUSTOM_TEST_FRAMEWORK_HPP
#define CUSTOM_TEST_FRAMEWORK_HPP

#include <iostream>
#include <vector>
#include <string>

#define TEST_CASE(name)                                                         \
    static void test_case_func_##__LINE__();                                    \
    namespace {                                                                 \
        struct test_case_registrar_##__LINE__ {                                 \
            test_case_registrar_##__LINE__() {                                  \
                CustomTestFramework::instance().registerTest(name, &test_case_func_##__LINE__); \
            }                                                                   \
        };                                                                      \
    }                                                                           \
    static test_case_registrar_##__LINE__ test_case_instance_##__LINE__;      \
    static void test_case_func_##__LINE__()

#define REQUIRE(condition) \
    if (!(condition)) { \
        std::cerr << "Requirement failed: " #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        exit(1); \
    }

class CustomTestFramework {
public:
    static CustomTestFramework& instance() {
        static CustomTestFramework instance;
        return instance;
    }

    void registerTest(const std::string& name, void (*test)()) {
        tests_.push_back({name, test});
    }

    int runAllTests() {
        int failed = 0;
        for (const auto& test : tests_) {
            try {
                test.test();
                std::cout << "[PASSED] " << test.name << std::endl;
            } catch (...) {
                std::cerr << "[FAILED] " << test.name << std::endl;
                failed++;
            }
        }
        return failed;
    }

private:
    struct Test {
        std::string name;
        void (*test)();
    };
    std::vector<Test> tests_;
};

#endif // CUSTOM_TEST_FRAMEWORK_HPP
