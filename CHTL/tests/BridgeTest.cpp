#define CATCH_CONFIG_MAIN
#include "bridge/ConcreteSaltBridge.h"
#include "catch.hpp"
#include <string>

TEST_CASE("SaltBridge data transfer", "[SaltBridge]") {
  CHTL::ConcreteSaltBridge bridge;
  std::string test_key = "test_data";
  std::string test_value = "hello world";

  bridge.sendData(test_key, test_value);
  auto received_data = bridge.requestData(test_key);

  REQUIRE(received_data.has_value());
  REQUIRE(std::any_cast<std::string>(*received_data) == test_value);
}

TEST_CASE("SaltBridge string transfer", "[SaltBridge]") {
  CHTL::ConcreteSaltBridge bridge;
  std::string test_key = "test_string";
  std::string test_value = "hello ctholly";

  bridge.sendString(test_key, test_value);
  auto received_value = bridge.requestString(test_key);

  REQUIRE(received_value.has_value());
  REQUIRE(*received_value == test_value);
}

TEST_CASE("SaltBridge request non-existent key", "[SaltBridge]") {
  CHTL::ConcreteSaltBridge bridge;
  auto received_data = bridge.requestData("non_existent_key");
  auto received_string = bridge.requestString("non_existent_key");

  REQUIRE_FALSE(received_data.has_value());
  REQUIRE_FALSE(received_string.has_value());
}

TEST_CASE("SaltBridge request string from non-string data", "[SaltBridge]") {
  CHTL::ConcreteSaltBridge bridge;
  std::string test_key = "test_int";
  int test_value = 123;

  bridge.sendData(test_key, test_value);
  auto received_string = bridge.requestString(test_key);

  REQUIRE_FALSE(received_string.has_value());
}
