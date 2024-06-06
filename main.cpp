#include "include/jsoncpp.h"
#include "include/testframe.h"

#include <string>
#include <cassert>

int main() {
    JSON::Json json;
    std::string filename = "example.json";
    json.ReadFile(filename);
    json.PrintRowjson();
}