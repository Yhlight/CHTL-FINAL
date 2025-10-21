#include "CJMOD/loader.h"
#include <dlfcn.h>
#include <stdexcept>

namespace CHTL {
namespace CJMOD {

void Loader::Load(const std::string& path, CHTLJS::ProgramNode& program) {
    void* handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle) {
        throw std::runtime_error("Cannot open CJMOD module: " + std::string(dlerror()));
    }

    // Reset errors
    dlerror();

    // Define the type of the init function
    typedef void (*init_func_t)(CHTLJS::ProgramNode&);

    // Look for the init function in the loaded module
    init_func_t init_func = (init_func_t) dlsym(handle, "chtl_cjmod_init");

    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        dlclose(handle);
        throw std::runtime_error("Cannot find symbol 'chtl_cjmod_init': " + std::string(dlsym_error));
    }

    // Call the init function
    init_func(program);

    // We're intentionally "leaking" the handle here.
    // The loaded module should remain in memory for the lifetime of the program.
}

} // namespace CJMOD
} // namespace CHTL
