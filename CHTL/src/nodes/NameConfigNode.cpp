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

    std::unique_ptr<AstNode> NameConfigNode::clone() const
    {
        auto node = std::make_unique<NameConfigNode>();
        node->settings = this->settings;
        return node;
    }

    void NameConfigNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t settings_count = settings.size();
        os.write(reinterpret_cast<const char*>(&settings_count), sizeof(settings_count));
        for(const auto& pair : settings)
        {
            size_t key_len = pair.first.length();
            os.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
            os.write(pair.first.c_str(), key_len);

            size_t val_len = pair.second.length();
            os.write(reinterpret_cast<const char*>(&val_len), sizeof(val_len));
            os.write(pair.second.c_str(), val_len);
        }
    }

    std::unique_ptr<NameConfigNode> NameConfigNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<NameConfigNode>();
        size_t settings_count;
        is.read(reinterpret_cast<char*>(&settings_count), sizeof(settings_count));
        for(size_t i = 0; i < settings_count; ++i)
        {
            size_t key_len;
            is.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
            std::string key;
            key.resize(key_len);
            is.read(&key[0], key_len);

            size_t val_len;
            is.read(reinterpret_cast<char*>(&val_len), sizeof(val_len));
            std::string value;
            value.resize(val_len);
            is.read(&value[0], val_len);
            node->settings[key] = value;
        }
        return node;
    }
}