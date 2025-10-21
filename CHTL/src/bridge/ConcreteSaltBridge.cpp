#include "bridge/ConcreteSaltBridge.h"

namespace CHTL {

void ConcreteSaltBridge::sendData(const std::string& key, std::any data) {
    data_store_[key] = data;
}

std::any ConcreteSaltBridge::requestData(const std::string& key) {
    if (data_store_.count(key)) {
        return data_store_.at(key);
    }
    return {};
}

}
