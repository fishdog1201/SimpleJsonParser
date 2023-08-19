#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include "json.h"

namespace json
{
    class JObject;

    class Parser
    {
    public:
        Parser() = default;

        static JObject fromString(std::string_view content);

        template<typename T>
        static std::string toJson(T& src)
        {
            if constexpr (is_basic_type<T>()) {
                JObject object = src;
                return object.to_string();
            }
            JObject object = std::map<std::string, JObject>();
            src._to_json(object);
            return object.to_string();
        }

        template<typename T>
        static T fromJson(std::string_view src)
        {
            JObject object = fromString(src);
            if constexpr (is_basic_type<T>()) {
                return object.template Value<T>();
            }
            if (object.type() != JSON_DICT) {
                throw std::logic_error("[fromJson]Not a dict type!");
            }
            T ret;
            ret._from_json(object);
            return ret;
        }

        void init(std::string_view src);

        void trim_right();

        void skip_comment();

        bool is_esc_consume(size_t pos);

        char get_next_token();

        JObject parse();

        JObject parse_null();

        JObject parse_number();

        JObject parse_list();

        JObject parse_string();

        JObject parse_dict();

        bool parse_bool();
    private:
        std::string m_str;
        size_t m_idx{};
    };
}

