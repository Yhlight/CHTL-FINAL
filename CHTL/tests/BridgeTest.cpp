#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "bridge/ConcreteSaltBridge.h"
#include <string>

TEST_CASE("SaltBridge data transfer", "[SaltBridge]") {
    CHTL::ConcreteSaltBridge bridge;
    std::string test_key = "test_data";
    std::string test_value = "hello world";

    bridge.sendData(test_key, test_value);
    std::any received_data = bridge.requestData(test_key);

    REQUIRE(received_data.has_value());
    REQUIRE(std::any_cast<std::string>(received_data) == test_value);
}
