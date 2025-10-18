#include "NameConfigNode.h"
#include <sstream>

namespace CHTL
{
    std::string NameConfigNode::ToString() const
    {
        std::stringstream ss;
        ss << "NameConfigNode(settings: [";
        for (auto it = settings.begin(); it != settings.end(); ++it)
        {
            ss << it->first << ": " << it->second;
            if (std::next(it) != settings.end())
            {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }
}