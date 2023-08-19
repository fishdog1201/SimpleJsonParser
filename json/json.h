#pragma once

#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <stdexcept>


namespace json {
    enum Type {
        JSON_NULL = 0,
        JSON_BOOL,
        JSON_INT,
        JSON_DOUBLE,
        JSON_STR,
        JSON_LIST,
        JSON_DICT
    };

    class JObject;

    template<typename T>
    constexpr bool is_basic_type()
    {
        if constexpr(std::is_same<T, std::string>::value ||
                     std::is_same<T, bool>::value ||
                     std::is_same<T, double>::value ||
                     std::is_same<T, int>::value){
            return true;
        }
        return false;
    }

    class JObject
    {
    public:
        using value_t = std::variant<bool,
                                     int,
                                     double,
                                     std::string,
                                     std::vector<JObject>,
                                     std::map<std::string, JObject>>;

        JObject()
            : m_type(JSON_NULL), m_value("null") {}
        JObject(int value)
        {
            Int(value);
        }
        JObject(bool value)
        {
            Bool(value);
        }
        JObject(double value)
        {
            Double(value);
        }
        JObject(std::string value)
        {
            Str(value);
        }
        JObject(const char* value)
        {
            Str(value);
        }
        JObject(std::vector<JObject> value)
        {
            List(value);
        }
        JObject(std::map<std::string, JObject> value)
        {
            Dict(std::move(value));
        }

        void Null()
        {
            m_type = JSON_NULL;
            m_value = "null";
        }
        void Int(int value)
        {
            m_type = JSON_INT;
            m_value = value;
        }
        void Bool(bool value)
        {
            m_type = JSON_BOOL;
            m_value = value;
        }
        void Double(double value)
        {
            m_type = JSON_DOUBLE;
            m_value = value;
        }
        void Str(std::string_view value)
        {
            m_type = JSON_STR;
            m_value = std::string(value);
        }
        void List(std::vector<JObject> value)
        {
            m_type = JSON_LIST;
            m_value = std::move(value);
        }
        void Dict(std::map<std::string, JObject> value)
        {
            m_type = JSON_DICT;
            m_value = std::move(value);
        }

#define THROW_GET_ERROR(erron) throw std::logic_error("Type error in get "#erron" value!")

        template<typename T>
        T& Value()
        {
            if constexpr(std::is_same<T, std::string>::value) {
                if (m_type != JSON_STR) {
                    THROW_GET_ERROR(string);
                }
            } else if constexpr(std::is_same<T, int>::value) {
                if (m_type != JSON_INT) {
                    THROW_GET_ERROR(INT);
                }
            } else if constexpr(std::is_same<T, bool>::value) {
                if (m_type != JSON_BOOL) {
                    THROW_GET_ERROR(BOOL);
                }
            } else if constexpr(std::is_same<T, double>::value) {
                if (m_type != JSON_DOUBLE) {
                    THROW_GET_ERROR(DOUBLE);
                }
            } else if constexpr(std::is_same<T, std::vector<JObject>>::value) {
                if (m_type != JSON_LIST) {
                    THROW_GET_ERROR(LIST);
                }
            } else if constexpr(std::is_same<T, std::map<std::string, JObject>>::value) {
                if (m_type != JSON_DICT) {
                    THROW_GET_ERROR(DICT);
                }
            }

            void *v = value();
            if (v == nullptr) {
                throw std::logic_error("Unknown type in JObject::Value()");
            }

            return *((T*)v);
        }

        Type type()
        {
            return m_type;
        }

        std::string to_string();

        void push_back(JObject item)
        {
            if (m_type == JSON_LIST) {
                auto& list = Value<std::vector<JObject>>();
                list.push_back(std::move(item));
                return;
            }
            throw std::logic_error("[JObject::push_back]Not a list type!");
        }

        void pop_back()
        {
            if (m_type == JSON_LIST) {
                auto& list = Value<std::vector<JObject>>();
                list.pop_back();
                return;
            }
            throw std::logic_error("[JObject::pop_back]Not a list type!");
        }

        JObject& operator[](std::string const& key)
        {
            if (m_type == JSON_DICT) {
                auto& dict = Value<std::map<std::string, JObject>>();
                return dict[key];
            }
            throw std::logic_error("[JObject::operator[]]Not a dict type!");
        }

    private:
        void *value();

    private:
        Type m_type;
        value_t m_value;
    };
}

