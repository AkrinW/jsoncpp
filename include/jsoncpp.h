#ifndef JSONCPP_H
#define JSONCPP_H

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <list>
#include <stack>

namespace JSON
{

enum jsonType {
    _NULL,
    _BOOLEAN,
    _NUMBER,
    _STRING,
    _ARRAY,
    _OBJECT
};

class Json {
private:
    struct jsonNode;
    using Valuetype = std::variant<std::nullptr_t, std::string, bool, 
                            double, jsonNode*>;
    using jsonArrayValue = std::variant<std::nullptr_t, bool, 
                            double, std::string, jsonNode*>;
    using jsonValue = std::variant<std::nullptr_t, bool, double, std::string, 
                            std::vector<jsonArrayValue>, jsonNode*>;
    std::string rowjson;
    jsonNode *root;
    std::string curKey;//指向当前的key
    jsonNode *curNode;//指向当前的node
    std::unordered_map<char,int> map;
    std::list<char> list;
    std::stack<char> BracketStack;//字符串栈
    std::stack<jsonNode*> NodeStack;//结点栈，用于读取当前结点。

    void InitNode();
    void printBool();
    void printDouble();
    bool IfValid(std::string json);
public:
    Json();
    ~Json();
    void ReadFile(std::string filename);
    void PrintRowjson();
    void AddToMap();
    void AddNode();
    void Build();
};

} // namespace json

#endif