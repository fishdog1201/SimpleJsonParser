#include <iostream>
#include "json.h"
#include "parser.h"
#include <fstream>

struct Base
{
    int xx;
    std::string yy;

    void _from_json(json::JObject& obj)
    {
        xx = obj["xx"].Value<int>();
        yy = obj["yy"].Value<std::string>();
    }

    void _to_json(json::JObject& obj)
    {
        obj["xx"] = xx;
        obj["yy"] = yy;
    }
};

struct MyTest
{
    int id;
    std::string name;
    Base q;

    void _to_json(json::JObject& obj)
    {
        json::JObject tmp = std::map<std::string, json::JObject>();
        q._to_json(tmp);
        obj["base"] = tmp;
    }

    void _from_json(json::JObject& obj)
    {
        id = obj["id"].Value<int>();
        name = obj["name"].Value<std::string>();
        q._from_json(obj["base"]);
    }
};

void test_class_serialization()
{
    MyTest test{.id = 32, .name = "jks"};
    auto item = json::Parser::fromJson<MyTest>(R"({"base":{"pp":0,"qq":""},"id":32,"name":"fda"} )");
    std::cout << json::Parser::toJson(item);
}

void test_string_parser()
{
    std::ifstream fin(R"(D:\CPP\SimpleJSONParser\SimpleJsonParser\test_source\test.json)");
    if (!fin) {
        std::cout << "read json file failed!\n";
        return;
    }

    std::string text((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    auto object = json::Parser::fromString(text);
    std::cout << (object["[css]"].to_string()) << '\n';
}

int main()
{
    test_string_parser();

    return 0;
}
