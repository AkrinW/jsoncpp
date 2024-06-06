#include "include/jsoncpp.h"
#include <string>

int main() {
    JSON::Json json;
    std::string filename = "example.json";
    json.ReadFile(filename);
    json.PrintRowjson();
    return 0;
}