#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_map>
#include "nodes/ExpressionNode.h"
#include "CHTLJS/include/nodes/AnimateNode.h"

namespace CHTL
{
    class Serializer
    {
    public:
        template <typename T>
        static void serialize(const T& value, std::ostream& os);

        template <typename T>
        static void deserialize(T& value, std::istream& is);

        template <typename T>
        static void serialize(const std::vector<T>& vec, std::ostream& os)
        {
            size_t size = vec.size();
            os.write(reinterpret_cast<const char*>(&size), sizeof(size));
            for (const auto& item : vec)
            {
                serialize(item, os);
            }
        }

        template <typename T>
        static void deserialize(std::vector<T>& vec, std::istream& is)
        {
            size_t size;
            is.read(reinterpret_cast<char*>(&size), sizeof(size));
            vec.resize(size);
            for (size_t i = 0; i < size; ++i)
            {
                deserialize(vec[i], is);
            }
        }

        template <typename K, typename V>
        static void serialize(const std::map<K, V>& map, std::ostream& os)
        {
            size_t size = map.size();
            os.write(reinterpret_cast<const char*>(&size), sizeof(size));
            for (const auto& pair : map)
            {
                serialize(pair.first, os);
                serialize(pair.second, os);
            }
        }

        template <typename K, typename V>
        static void deserialize(std::map<K, V>& map, std::istream& is)
        {
            size_t size;
            is.read(reinterpret_cast<char*>(&size), sizeof(size));
            for (size_t i = 0; i < size; ++i)
            {
                K key;
                V value;
                deserialize(key, is);
                deserialize(value, is);
                map[key] = std::move(value);
            }
        }

        template <typename K, typename V>
        static void serialize(const std::unordered_map<K, V>& map, std::ostream& os)
        {
            size_t size = map.size();
            os.write(reinterpret_cast<const char*>(&size), sizeof(size));
            for (const auto& pair : map)
            {
                serialize(pair.first, os);
                serialize(pair.second, os);
            }
        }

        template <typename K, typename V>
        static void deserialize(std::unordered_map<K, V>& map, std::istream& is)
        {
            size_t size;
            is.read(reinterpret_cast<char*>(&size), sizeof(size));
            for (size_t i = 0; i < size; ++i)
            {
                K key;
                V value;
                deserialize(key, is);
                deserialize(value, is);
                map[key] = std::move(value);
            }
        }

    };

}
