#ifndef CHTL_CMOD_PACKER_H
#define CHTL_CMOD_PACKER_H

#include "nodes/ProgramNode.h"
#include <string>

namespace CHTL {
namespace CMOD {

class Packer {
public:
    static void Pack(const ProgramNode& program, const std::string& output_path);
};

} // namespace CMOD
} // namespace CHTL

#endif // CHTL_CMOD_PACKER_H
