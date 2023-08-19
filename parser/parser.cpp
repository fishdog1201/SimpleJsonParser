#include "json.h"
#include "parser.h"
#include <cctype>
#include <algorithm>

json::JObject json::Parser::fromString(std::string_view content)
{
    static Parser instance;
    instance.init(content);
    return instance.parse();
}

void json::Parser::init(std::string_view src)
{
    m_str = src;
    m_idx = 0;
    trim_right();
}

void json::Parser::trim_right()
{
    m_str.erase(std::find_if(m_str.rbegin(), m_str.rend(), [](char ch)
    {
        return !std::isspace(ch);
    }).base(), m_str.end());
}

void json::Parser::skip_comment()
{
    if (m_str.compare(m_idx, 2, R"(//)") == 0) {
        while(true) {
            auto next_pos = m_str.find('\n', m_idx);
            if (next_pos == std::string::npos) {
                throw std::logic_error("[skip_comment]invalid comment area.");
            }
            m_idx = next_pos + 1;
            while(std::isspace(m_str[m_idx])) {
                ++m_idx;
            }
            if (m_str.compare(m_idx, 2, R"(//)") == 0) {
                return;
            }
        }
    }
}

char json::Parser::get_next_token()
{
    while(std::isspace(m_str[m_idx])) {
        ++m_idx;
    }

    if (m_idx >= m_str.size()) {
        throw std::logic_error("Unexpect charactor in json content.");
    }
    skip_comment();
    return m_str[m_idx];
}

bool json::Parser::is_esc_consume(size_t pos)
{
    size_t end_pos = pos;
    while(m_str[pos] == '\\') pos--;
    auto cnt = end_pos - pos;
    return cnt % 2 == 0;
}

json::JObject json::Parser::parse()
{
    char token = get_next_token();
    if (token == 'n')
    {
        return parse_null();
    }
    if (token == 't' || token == 'f')
    {
        return parse_bool();
    }
    if (token == '-' || std::isdigit(token))
    {
        return parse_number();
    }
    if (token == '\"')
    {
        return parse_string();
    }
    if (token == '[')
    {
        return parse_list();
    }
    if (token == '{')
    {
        return parse_dict();
    }

    throw std::logic_error("unexpected character in parse json");
}

json::JObject json::Parser::parse_null()
{
    if (m_str.compare(m_idx, 4, "null") == 0) {
        m_idx += 4;
        return {};
    }
    throw std::logic_error("Parse null error");
}

json::JObject json::Parser::parse_number()
{
    auto pos = m_idx;
    if (m_str[m_idx] == '-') {
        m_idx++;
    }
    if (std::isdigit(m_str[m_idx])) {
        while (std::isdigit(m_str[m_idx])) {
            m_idx++;
        }
    } else {
        throw std::logic_error("invalid character in number");
    }

    if (m_str[m_idx] != '.') {
        return (int)strtol(m_str.c_str()+pos, nullptr, 10);
    }

    if (m_str[m_idx] == '.')
    {
        m_idx++;
        if (!std::isdigit(m_str[m_idx]))
        {
            throw std::logic_error("at least one digit required in parse float part!");
        }
        while (std::isdigit(m_str[m_idx]))
            m_idx++;
    }
    return strtod(m_str.c_str() + pos, nullptr);
}

json::JObject json::Parser::parse_string()
{
    auto pre_pos = ++m_idx;
    auto pos = m_str.find('"', m_idx);

    if (pos != std::string::npos) {
        while(true) {
            if (m_str[pos-1] != '\\') {
                break;
            }
            if (is_esc_consume(pos-1)) {
                break;
            }
            pos = m_str.find('"', pos+1);
            if (pos == std::string::npos) {
                throw std::logic_error(R"(expected left '"' in parse string!)");
            }
        }
        m_idx = pos + 1;
        return m_str.substr(pre_pos, pos-pre_pos);
    }
    throw std::logic_error("Parse string error!");
}

bool json::Parser::parse_bool()
{
    if (m_str.compare(m_idx, 4, "true") == 0) {
        m_idx += 4;
        return true;
    } else if (m_str.compare(m_idx, 5, "false") == 0) {
        m_idx += 5;
        return false;
    }
    throw std::logic_error("Parse bool error.");
}

json::JObject json::Parser::parse_list()
{
    JObject arr = std::vector<JObject>();
    m_idx++;
    char ch = get_next_token();
    if (ch == ']') {
        m_idx++;
        return arr;
    }

    while(true) {
        arr.push_back(parse());
        ch = get_next_token();
        if (ch == ']') {
            m_idx++;
            break;
        }
        if (ch != ',') {
            throw std::logic_error("Expected ',' in parse list.");
        }
        m_idx++;
    }
    return arr;
}

json::JObject json::Parser::parse_dict()
{
    JObject dict = std::map<std::string, JObject>();
    m_idx++;
    char ch = get_next_token();
    if (ch == '}') {
        m_idx++;
        return dict;
    }

    while(true) {
        std::string key = std::move(parse().Value<std::string>());
        ch = get_next_token();
        if (ch != ':') {
            throw std::logic_error("Expected ':' in parse dict.");
        }
        m_idx++;
        dict[key] = parse();
        ch = get_next_token();
        if (ch == '}') {
            m_idx++;
            break;
        }
        if (ch != ',') {
            throw std::logic_error("Expected ',' in parse dict.");
        }
        m_idx++;
    }
    return dict;
}