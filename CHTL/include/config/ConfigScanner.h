#ifndef CHTL_CONFIGSCANNER_H
#define CHTL_CONFIGSCANNER_H

#include <string>

namespace CHTL
{

    class ConfigScanner
    {
    public:
        ConfigScanner(const std::string &source);

        void Scan();

    private:
        std::string m_source;
        size_t m_cursor = 0;

        void SkipWhitespace();
        bool Match(const std::string &expected);
        std::string ParseIdentifier();
        std::string ParseStringLiteral();

        void ParseConfigurationBlock();
        void ParseNameBlock();
    };

}

#endif // CHTL_CONFIGSCANNER_H
