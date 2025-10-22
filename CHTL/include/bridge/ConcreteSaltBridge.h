#ifndef CHTL_CONCRETE_SALT_BRIDGE_H
#define CHTL_CONCRETE_SALT_BRIDGE_H

#include "bridge/SaltBridge.h"
#include <unordered_map>

namespace CHTL {

class ConcreteSaltBridge : public SaltBridge {
public:
  void sendData(const std::string &key, std::any data) override;
  std::optional<std::any> requestData(const std::string &key) override;

  void sendString(const std::string &key, const std::string &value) override;
  std::optional<std::string> requestString(const std::string &key) override;

private:
  std::unordered_map<std::string, std::any> data_store_;
};

} // namespace CHTL

#endif