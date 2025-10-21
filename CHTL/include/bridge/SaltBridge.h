#ifndef CHTL_SALT_BRIDGE_H
#define CHTL_SALT_BRIDGE_H

#include <any>
#include <optional>
#include <string>

namespace CHTL {

class SaltBridge {
public:
  virtual ~SaltBridge() = default;

  virtual void sendData(const std::string &key, std::any data) = 0;
  virtual std::optional<std::any> requestData(const std::string &key) = 0;

  virtual void sendString(const std::string &key, const std::string &value) = 0;
  virtual std::optional<std::string> requestString(const std::string &key) = 0;
};

} // namespace CHTL

#endif