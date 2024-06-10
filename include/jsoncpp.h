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
    struct jsonValue;
    using Valuetype = std::variant<std::nullptr_t, std::string, bool, 
                            double, jsonNode*>;
    struct VariantTypeGetter;

    // using jsonArrayValue = std::variant<std::nullptr_t, bool, 
    //                         double, std::string, jsonNode*>;
    // using jsonValue = std::variant<std::nullptr_t, bool, double, std::string, 
    //                         std::vector<jsonArrayValue>, jsonNode*>;
    std::string rowjson;
    int rown;//rowjson's length
    std::unordered_map<char,int> map;
    std::list<char> list;

    jsonNode *root;
    std::string curKey;//指向当前的key
    jsonNode *curNode;//指向当前的node
    std::vector<jsonValue*> *curArray;//指向当前的数组

    std::stack<char> BracketStack;//字符串栈
    std::stack<jsonNode*> NodeStack;//结点栈，用于读取当前结点。
    std::stack<std::vector<jsonValue*>*> ArrayStack;//数组栈，用于读取当前数组。
    
    void InitNode();

    void printBool(jsonValue* value);
    void printDouble(jsonValue* value);
    void printString(jsonValue* value);
    void printNull(jsonValue* value);
    void printNode(jsonNode* node, int i);
    void printArray(jsonValue* node, int i);
    void printValue(jsonValue* value);
    void printValue(jsonValue* value, jsonType t);

    bool IfValid(std::string json);//解析json文件是否合法

    // 解析字符类型
    std::string getNextKey(int &i);
    jsonValue* getNextValue(int &i);
    std::string getString(int &i);
    double getDouble(int &i);
    bool getBool(int &i);
    std::nullptr_t getNull(int &i);
    jsonNode* getObject(int &i);
    std::vector<jsonValue*>* getArray(int &i);
    jsonType getValueType(jsonValue* p);

    std::string SaveAsString();
    std::string BooltoString(jsonValue* value);
    std::string DoubletoString(jsonValue* value);
    std::string StringtoString(jsonValue* value);
    std::string NulltoString(jsonValue* value);
    std::string NodetoString(jsonNode* node, int i);
    std::string ArraytoString(jsonValue* node, int i);

    void ShowNodeKeys(jsonNode* p, int numoftab, std::string name);
    void ShowArrayKeys(std::vector<jsonValue*>* a, int numoftab, std::string name);
    std::string ShowJsonType(jsonType type);

    void DeleteKeyInNode(std::string keyname, jsonNode *p);
    // void DeleteJsonValue(jsonValue *v, jsonType t);
    // void DeleteJsonValueInArray(jsonValue *v);
    void SearchKeyInNode(std::string keyname, jsonNode *p);
    
public:
    Json();
    ~Json();
    void ReadFile(std::string filename);
    void SaveFile(std::string filename);
    void PrintRowjson();
    void AddToMap();
    void AddNode();
    void Build();
    void PrintJson();
    void ShowKeys();
    void DeleteKey(std::string keyname);
    void SearchKey(std::string keyname);
};

} // namespace json

#endif