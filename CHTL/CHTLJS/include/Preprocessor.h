#ifndef CHTLJS_PREPROCESSOR_H
#define CHTLJS_PREPROCESSOR_H

#include <string>

namespace CHTLJS {

class Preprocessor {
public:
    static std::string process(const std::string& input);
};

} // namespace CHTLJS

#endif // CHTLJS_PREPROCESSOR_H
