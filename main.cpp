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
    // json.AddToMap();
    json.Build();
    json.PrintJson();

    // std::cout << "输入保存文件名：";
    // std::string savefilename;
    // std::cin >> savefilename;
    // json.SaveFile(savefilename);
    // json.DeleteKey("root.author[5]");
    // json.DeleteKey("root.publisher.Country");
    // json.PrintJson();
    // json.ShowKeys();
    // json.SearchKey("root.author");
    // json.SearchKey("root.publisher.Country");

    json.RewriteKey("root.publisher.Country","    {\n\"Company\":-10.92E-33,\n\"Country\": 3}");
    json.PrintJson();


    std::cout << "输入保存文件名：";
    std::string savefilename;
    std::cin >> savefilename;
    json.SaveFile(savefilename);
    return 0;
}
//example/example.json
//example/string.json
//example/empty.json
//example/wrongempty.json
//example/null.json
//example/bool.json
//example/num.json
//example/example_save.json