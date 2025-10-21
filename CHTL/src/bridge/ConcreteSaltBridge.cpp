#include "bridge/ConcreteSaltBridge.h"

namespace CHTL {

void ConcreteSaltBridge::sendData(const std::string &key, std::any data) {
  data_store_[key] = data;
}

std::optional<std::any>
ConcreteSaltBridge::requestData(const std::string &key) {
  if (data_store_.count(key)) {
    return data_store_.at(key);
  }
  return std::nullopt;
}

void ConcreteSaltBridge::sendString(const std::string &key,
                                    const std::string &value) {
  data_store_[key] = value;
}

std::optional<std::string>
ConcreteSaltBridge::requestString(const std::string &key) {
  if (data_store_.count(key)) {
    try {
      return std::any_cast<std::string>(data_store_.at(key));
    } catch (const std::bad_any_cast &e) {
      return std::nullopt;
    }
  }
  return std::nullopt;
}

} // namespace CHTL
