#include "ConfigurationNode.h"
#include "NameConfigNode.h"
#include <sstream>

namespace CHTL
{
    std::string ConfigurationNode::ToString() const
    {
        std::stringstream ss;
        ss << "ConfigurationNode(";
        if (!name.empty())
        {
            ss << "name: " << name << ", ";
        }
        ss << "settings: [";
        for (auto it = settings.begin(); it != settings.end(); ++it)
        {
            ss << it->first << ": " << it->second;
            if (std::next(it) != settings.end())
            {
                ss << ", ";
            }
        }
        ss << "]";
        if (name_config)
        {
            ss << ", name_config: " << name_config->ToString();
        }
        ss << ")";
        return ss.str();
    }

    std::unique_ptr<AstNode> ConfigurationNode::clone() const
    {
        auto node = std::make_unique<ConfigurationNode>();
        node->name = this->name;
        node->settings = this->settings;
        if (this->name_config)
        {
            node->name_config.reset(static_cast<NameConfigNode *>(this->name_config->clone().release()));
        }
        return node;
    }

    void ConfigurationNode::serialize(std::ostream& os) const
    {
        int type = static_cast<int>(GetType());
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));

        size_t name_len = name.length();
        os.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        os.write(name.c_str(), name_len);

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

        bool has_name_config = name_config != nullptr;
        os.write(reinterpret_cast<const char*>(&has_name_config), sizeof(has_name_config));
        if (has_name_config)
        {
            name_config->serialize(os);
        }
    }

    std::unique_ptr<ConfigurationNode> ConfigurationNode::deserialize(std::istream& is)
    {
        auto node = std::make_unique<ConfigurationNode>();

        size_t name_len;
        is.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        node->name.resize(name_len);
        is.read(&node->name[0], name_len);

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

        bool has_name_config;
        is.read(reinterpret_cast<char*>(&has_name_config), sizeof(has_name_config));
        if (has_name_config)
        {
            node->name_config = NameConfigNode::deserialize(is);
        }
        return node;
    }
}