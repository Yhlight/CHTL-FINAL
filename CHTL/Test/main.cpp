#include "catch.hpp"

int main(int argc, char* argv[]) {
    return Catch::TestRegistry::instance().run_all_tests();
}
