#ifndef CHTL_SALT_BRIDGE_H
#define CHTL_SALT_BRIDGE_H

#include <string>
#include <any>

namespace CHTL {

class SaltBridge {
public:
    virtual ~SaltBridge() = default;

    virtual void sendData(const std::string& key, std::any data) = 0;
    virtual std::any requestData(const std::string& key) = 0;
};

}

#endif
