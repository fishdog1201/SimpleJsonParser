#include <iostream>
#include "json.h"

int main()
{
    json::JObject v1;
    json::JObject v2 = true;
    json::JObject v3 = 2;
    json::JObject v4 = 1.23;
    json::JObject v5 = "hello";

    return 0;
}
