#include "include/jsoncpp.h"
#include "include/testframe.h"

#include <cassert>

int main() {
    JSON::Json json;
    std::string filename;
    std::cin >> filename;
    json.ReadFile(filename);
    json.PrintRowjson();
    json.AddToMap();
    json.Build();

    return 0;
}
//example/example.json
//example/string.json
//example/empty.json
//example/wrongempty.json