#ifndef JSONCPP_H
#define JSONCPP_H

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <list>
#include <stack>
#include <memory>

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

    //not use now;
    // using Valuetype = std::variant<std::nullptr_t, std::string, bool, 
    //                         double, ptr_jsonNode>;

    struct VariantTypeGetter;

    using ptr_jsonValue = std::shared_ptr<jsonValue>;
    using ptr_jsonNode = std::shared_ptr<jsonNode>;
    using ptr_jsonArray = std::shared_ptr<std::vector<ptr_jsonValue>>;

    // using jsonArrayValue = std::variant<std::nullptr_t, bool, 
    //                         double, std::string, ptr_jsonNode>;
    // using jsonValue = std::variant<std::nullptr_t, bool, double, std::string, 
    //                         std::vector<jsonArrayValue>, ptr_jsonNode>;

    std::string rowjson;
    int rown;//rowjson's length
    std::unordered_map<char,int> map;
    std::list<char> list;

    ptr_jsonNode root;
    std::string curKey;//指向当前的key
    ptr_jsonNode curNode;//指向当前的node
    ptr_jsonArray curArray;//指向当前的数组

    std::stack<char> BracketStack;//字符串栈
    std::stack<ptr_jsonNode> NodeStack;//结点栈，用于读取当前结点。
    std::stack<ptr_jsonArray> ArrayStack;//数组栈，用于读取当前数组。
    
    void InitNode();
    void AddToMap();
    void AddNode();

    void printBool(ptr_jsonValue value);
    void printDouble(ptr_jsonValue value);
    void printString(ptr_jsonValue value);
    void printNull(ptr_jsonValue value);
    void printNode(ptr_jsonNode node, int i);
    void printArray(ptr_jsonValue value, int i);
    void printValue(ptr_jsonValue value);
    void printValue(ptr_jsonValue value, jsonType t);

    bool IfValid(std::string json);//解析json文件是否合法

    // 解析字符类型
    std::string getNextKey(int &i, std::string s = "");
    ptr_jsonValue getNextValue(int &i, std::string s = "");
    std::string getString(int &i, std::string s = "");
    double getDouble(int &i, std::string s = "");
    bool getBool(int &i, std::string s = "");
    std::nullptr_t getNull(int &i);
    ptr_jsonNode getObject(int &i);
    ptr_jsonArray getArray(int &i);
    jsonType getValueType(ptr_jsonValue p);

    std::string SaveAsString();
    std::string BooltoString(ptr_jsonValue value);
    std::string DoubletoString(ptr_jsonValue value);
    std::string StringtoString(ptr_jsonValue value);
    std::string NulltoString(ptr_jsonValue value);
    std::string NodetoString(ptr_jsonNode node, int i);
    std::string ArraytoString(ptr_jsonValue value, int i);

    void ShowNodeKeys(ptr_jsonNode p, int numoftab, std::string name);
    void ShowArrayKeys(ptr_jsonArray a, int numoftab, std::string name);
    std::string ShowJsonType(jsonType type);

    void DeleteKeyInNode(std::string keyname, ptr_jsonNode p);
    // void DeleteJsonValue(jsonValue *v, jsonType t);
    // void DeleteJsonValueInArray(jsonValue *v);
    void SearchKeyInNode(std::string keyname, ptr_jsonNode p);
    void InsertKeyInNode(std::string keyname, std::string value, ptr_jsonNode p);
    ptr_jsonValue StringtoValue(std::string s);
public:
    Json();
    ~Json();
    void ReadFile(std::string filename);
    void SaveFile(std::string filename);
    void PrintRowjson();
    void Build(std::string s = "");
    void PrintJson();
    void ShowKeys();
    void DeleteKey(std::string keyname);
    void SearchKey(std::string keyname);
    void InsertKey(std::string keyname, std::string value);
};

} // namespace json

#endif