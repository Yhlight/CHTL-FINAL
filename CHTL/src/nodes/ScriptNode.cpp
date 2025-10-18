#include "nodes/ScriptNode.h"

namespace CHTL
{
    std::string ScriptNode::ToString() const
    {
        // For debugging, show a snippet of the content.
        std::string content_snippet = content;
        if (content_snippet.length() > 20)
        {
            content_snippet = content_snippet.substr(0, 17) + "...";
        }
        // Replace newlines with a literal \n to keep the string on one line
        size_t pos = 0;
        while ((pos = content_snippet.find('\n', pos)) != std::string::npos) {
            content_snippet.replace(pos, 1, "\\n");
            pos += 2;
        }

        return "ScriptNode(content: \"" + content_snippet + "\")";
    }
}