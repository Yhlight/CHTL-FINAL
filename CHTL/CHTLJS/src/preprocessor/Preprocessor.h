#pragma once

#include <string>

namespace CHTLJS
{
    class Preprocessor
    {
    public:
        /**
         * @brief Processes a string containing mixed CHTL JS and JavaScript code,
         * wrapping CHTL JS blocks with special markers.
         * @param input The raw source code.
         * @return The processed source code.
         */
        static std::string Process(const std::string& input);
    };

} // namespace CHTLJS
