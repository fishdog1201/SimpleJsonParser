#include "json.h"
#include <sstream>

void *json::JObject::value()
{
    switch (m_type) {
        case JSON_NULL:
            return std::get_if<std::string>(&m_value);
        case JSON_BOOL:
            return std::get_if<bool>(&m_value);
        case JSON_INT:
            return std::get_if<int>(&m_value);
        case JSON_DOUBLE:
            return std::get_if<double>(&m_value);
        case JSON_STR:
            return std::get_if<std::string>(&m_value);
        case JSON_LIST:
            return std::get_if<std::vector<JObject>>(&m_value);
        case JSON_DICT:
            return std::get_if<std::map<std::string, JObject>>(&m_value);
        default:
            return nullptr;
    }
}

#define GET_VALUE(type) *((type*) value)

std::string json::JObject::to_string()
{
    void *value = this->value();
    std::ostringstream os;

    switch(m_type) {
        case JSON_NULL:
            os << "null";
            break;
        case JSON_BOOL:
            if (GET_VALUE(bool)) {
                os << "true";
            } else {
                os << "false";
            }
            break;
        case JSON_INT:
            os << GET_VALUE(int);
            break;
        case JSON_DOUBLE:
            os << GET_VALUE(double);
            break;
        case JSON_STR:
            os << '\"' << GET_VALUE(std::string) << '\"';
            break;
        case JSON_LIST: {
            std::vector<JObject> &list = GET_VALUE(std::vector<JObject>);
            os << '[';

            for (int i = 0; i < list.size(); ++i) {
                if (i != list.size() - 1) {
                    os << list[i].to_string() << ',';
                } else {
                    os << list[i].to_string();
                }
            }
            os << ']';
            break;
        }
        case JSON_DICT: {
            std::map<std::string, JObject> &dict = *((std::map<std::string, JObject>*)value);
            os << '{';

            for (auto it = dict.begin(); it != dict.end(); ++it) {
                if (it != dict.begin()) {
                    os << ',';
                }
                os << '\"' << it->first << ':' << it->second.to_string() << '\"';
            }
            os << '}';
            break;
        }
        default:
            return "";
    }
    return os.str();
}