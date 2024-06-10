#include "include/jsoncpp.h"
#include "include/testframe.h"

#include <cassert>

int main() {
    JSON::Json json;
    std::string filename;
    std::cout << "输入文件名: ";
    std::cin >> filename;
    json.ReadFile(filename);
    json.PrintRowjson();
    json.AddToMap();
    json.Build();
    json.PrintJson();
    // std::cout << "输入保存文件名：";
    // std::string savefilename;
    // std::cin >> savefilename;
    // json.SaveFile(savefilename);
    json.ShowKeys();
    return 0;
}
//example/example.json
//example/string.json
//example/empty.json
//example/wrongempty.json
//example/null.json
//example/bool.json
//example/num.json