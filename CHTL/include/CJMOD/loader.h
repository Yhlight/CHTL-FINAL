#ifndef CHTL_CJMOD_LOADER_H
#define CHTL_CJMOD_LOADER_H

#include <string>
#include <memory>
#include "CHTLJS/include/nodes/ProgramNode.h"

namespace CHTL {
namespace CJMOD {

class Loader {
public:
    static void Load(const std::string& path, CHTLJS::ProgramNode& program);
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_LOADER_H
