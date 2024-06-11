#include "../include/jsoncpp.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <typeinfo>


namespace JSON{

//获取variant类型
struct Json::VariantTypeGetter {
    template <typename T>
    const char* operator()(T&&) const {
        return typeid(T).name();
    }
};
//  std::string typeName = std::visit(VariantTypeGetter{}, curNode->map[curKey]);
//  std::cout << "The type of variant is: " << typeName << std::endl;
// typeName = {
//     NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE;
//     d;
//     St10shared_ptrISt6vectorIS_IN4JSON4Json9jsonValueEESaIS4_EEE;
//     b;
//     Dn;
//     St10shared_ptrIN4JSON4Json8jsonNodeEE;
// }


struct Json::jsonValue {
    std::variant<std::nullptr_t, std::string, bool, 
                double, ptr_jsonNode, ptr_jsonArray> value;
    jsonValue() {}
    jsonValue(std::nullptr_t nptr): value(nptr){}
    jsonValue(bool flag): value(flag){}
    jsonValue(std::string s): value(s){}
    jsonValue(double d): value(d){}
    jsonValue(ptr_jsonNode n): value(n){}
    jsonValue(ptr_jsonArray v): value(v){}
    ~jsonValue();
};

struct Json::jsonNode {
    std::unordered_map<std::string, ptr_jsonValue> map;
    std::list<std::string> list;
    std::unordered_map<std::string, jsonType> typemap;
    // std::unordered_map<std::string, jsonValue> map;
    ~jsonNode();
};

Json::jsonValue::~jsonValue() {
    if (std::holds_alternative<ptr_jsonNode>(value)) {
        std::cout << "deletednode ";
    } else if (std::holds_alternative<ptr_jsonArray>(value)) {
        std::cout << "deltedArray ";
    } else if (std::holds_alternative<std::nullptr_t>(value)) {
        std::cout << "deletenull ";
    } else if (std::holds_alternative<double>(value)) {
        std::cout << "deltedouble ";
    } else if (std::holds_alternative<std::string>(value)) {
        std::cout << "deletestring ";
    } else if (std::holds_alternative<bool>(value)) {
        std::cout << "deletebool ";
    } else {
        std::cout << "wrong type";
    }
    value = nullptr;
}

Json::jsonNode::~jsonNode() {
    list.clear();
    typemap.clear();
    map.clear();
}

Json::Json() {
    rowjson = "";
    root = nullptr;
    curNode = nullptr;
    curKey = "";
}

Json::~Json() {
    // delete root;
    // delete curNode;
}

void Json::ReadFile(std::string filename) {
    std::ifstream file;
    file.open(filename);
    std::string line;
    while(std::getline(file, line)) {
        rowjson.append(line);
        rowjson += '\n';
    }
    rown = rowjson.size();
}

//读取json文件后，检查是否合法，保证后续储存的都是有效json
// bool Json::IfValid(std::string json) {

// }

//在rowjson中，只包含了字母, 数字, {}, [], :, "", ',.' 
void Json::PrintRowjson() {
    std::cout << rowjson;
}

void Json::AddToMap() {
    for (auto u : rowjson) {
        if (!map.count(u)) {
            list.push_back(u);
        }
        ++map[u];
    }
    // for (auto u : list) {
    //     std::cout << u << ' ' << map[u] << std::endl;
    // }
}

void Json::InitNode() {
    // root = new jsonNode();
    root = std::make_shared<jsonNode>();
}

//no use now.
void Json::AddNode() {
    if (root == nullptr) {
        InitNode();
        curNode = root;
    } else {
        // curNode->map[curKey] = new jsonValue(new jsonNode());
        // // auto pval = std::get_if<jsonNode*>(&curNode->map[curKey].value);
        // curNode = std::get<jsonNode*>(curNode->map[curKey]->value);
    }
}

//getsometing() only used by build() with rowjson.
//now insert() also use getsomething to bulid jsonvalues.
std::string Json::getString(int &i, std::string s) {
    std::string tmp = "";
    if (s == "") {
        s = rowjson;
    }
    ++i;
    int j = i;
    while (s[i] != '"') {
        if (s[i] == '\\') {
            ++i;
            if (s[i] == 'n') {
                tmp += '\n';
            } else if (s[i] == 't') {
                tmp += '\t';
            } else if (s[i] == '\'') {
                tmp += '\'';
            } else if (s[i] == '\\') {
                tmp += '\\';
            } else if (s[i] == '"') {
                std::cout << "Invalid json file. (wrong '\\' position)" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        } else {
            tmp += s[i];
        }
        ++i;
    }
    ++i;
    return tmp;
}

double Json::getDouble(int &i, std::string s) {
    std::string tmp = "";
    if (s == "") {
        s = rowjson;
    }
    int j = i;
    while (s[i] != ' ' && s[i] != ',' 
            && s[i] != '\n' && s[i] != '}'
            && s[i] != ']') {
       ++i;
    }
    tmp = s.substr(j,i-j);
    return std::stod(tmp);
}

bool Json::getBool(int &i, std::string s) {
    if (s == "") {
        s = rowjson;
    }
    if (s[i] == 'f') {
        i += 5;
        return false;
    }
    i += 4;
    return true;
}

std::nullptr_t Json::getNull(int &i) {
    i += 4;
    return std::nullptr_t();
}

Json::ptr_jsonNode Json::getObject(int &i) {
    auto p = std::make_shared<jsonNode>();
    // jsonNode *p = new jsonNode();
    BracketStack.push('}');
    NodeStack.push(p);
    ++i;
    return p;
}

Json::ptr_jsonArray Json::getArray(int &i) {
    auto p = std::make_shared<std::vector<ptr_jsonValue>>();
    BracketStack.push(']');
    ArrayStack.push(p);
    ++i;
    return p;
}

std::string Json::getNextKey(int &i, std::string s) {
    std::string tmp = "";
    if (s == "") {
        s = rowjson;
    }
    int n = s.size();
    while (i < n) {
        if (s[i] == ' ') {
            ++i;
        } else if (s[i] == '\n') {
            ++i;
        } else if (s[i] == ',') {
            ++i;
        } else if (s[i] == '"') {
            tmp = getString(i,s);
            break;
        } else if (s[i] == '}') {
            break;
        } else if (s[i] == '\t') {
            ++i;
        }
    }
    return tmp;
}

Json::ptr_jsonValue Json::getNextValue(int &i, std::string s) {
    ptr_jsonValue p;
    if (s == "") {
        s = rowjson;
    }
    int n = s.size();
    while (i < n) {
        if (s[i] == ' ') {
            ++i;
        } else if (s[i] == '\n') {
            ++i;
        } else if (s[i] == ':') {
            ++i;
        } else if (s[i] == '"') {
            p = std::make_shared<jsonValue>(getString(i,s));
            break;
        } else if (s[i] == 't' || s[i] == 'f') {
            p = std::make_shared<jsonValue>(getBool(i,s));
            break;
        } else if (s[i] == 'n') {
            p = std::make_shared<jsonValue>(getNull(i));
            if (p == nullptr) {
                std::cout <<"jsonValue(null) == nullptr";
            }
            break;
        } else if (s[i] == '{') {
            p = std::make_shared<jsonValue>(getObject(i));
            break;
        } else if (s[i] == '-' || (s[i] >= '0' && s[i] <= '9')) {
            p = std::make_shared<jsonValue>(getDouble(i,s));
            break;
        } else if (s[i] == '[') {
            p = std::make_shared<jsonValue>(getArray(i));
            break;
        } else if (s[i] == ']') {
            p = nullptr;
            break;
        } else if (s[i] == '\t') {
            ++i;
        }
    }
    return p;

}

jsonType Json::getValueType(ptr_jsonValue p) {
    jsonType tmp;
    if (std::holds_alternative<ptr_jsonNode>(p->value)) {
        tmp = _OBJECT;
    } else if (std::holds_alternative<ptr_jsonArray>(p->value)) {
        tmp = _ARRAY;
    } else if (std::holds_alternative<std::nullptr_t>(p->value)) {
        tmp = _NULL;
    } else if (std::holds_alternative<double>(p->value)) {
        tmp = _NUMBER;
    } else if (std::holds_alternative<std::string>(p->value)) {
        tmp = _STRING;
    } else if (std::holds_alternative<bool>(p->value)) {
        tmp = _BOOLEAN;
    } else {
        std::cout << "wrong type";
    }
    return tmp;
}

void Json::Build(std::string s) {
    int i = 0;
    // while (BracketStack.empty()) {
    //     if (rowjson[i] == ' ') {
    //         ++i;
    //     } else if (rowjson[i] == '\n') {
    //         ++i;
    //     } else if (rowjson[i] == '{') {
    //         curNode = getObject(i);
    //         root = curNode;
    //     } 
    // }
    if (s == "") {
        s = rowjson;
    }
    int n = s.size();
    bool ifgetkey, ifgetvalue;//用于表示当前是读key还是读value
    //因为存在两个都不读的情况，所以需要设置2个。
    while (i < n) {
        if (BracketStack.empty()) {
            while (BracketStack.empty() && i < n) {
                if (s[i] == ' ') {
                    ++i;
                } else if (s[i] == '\n') {
                    ++i;
                } else if (s[i] == '{') {
                    curNode = getObject(i);
                    if (root == nullptr) {
                        root = curNode;
                    }
                    ifgetkey = true;
                    ifgetvalue = false;
                } else if (s[i] == '\t') {
                    ++i;
                }
            }
        } else {
            if (BracketStack.top() == '}') {//在Node里添加数据
                if (ifgetkey) {
                    curKey = getNextKey(i,s);
                    if (curKey == "") {
                        ++i;
                        BracketStack.pop();
                        NodeStack.pop();
                        if (NodeStack.empty()) {
                            curNode = nullptr;
                        } else {
                            curNode = NodeStack.top();
                        }
                        ifgetkey = false;
                        continue;
                    }
                    curNode->list.push_back(curKey);
                    ifgetkey = false;
                }
                if (ifgetvalue) {
                    auto p = getNextValue(i,s);
                    curNode->map[curKey] = p;
                    jsonType _tmptype = getValueType(p);
                    curNode->typemap[curKey] = _tmptype;
                    ifgetvalue = false;
                    if (_tmptype == _OBJECT) {
                        curNode = NodeStack.top();
                        ifgetkey = true;
                    } else if (_tmptype == _ARRAY) {
                        curArray = ArrayStack.top();
                        ifgetvalue = true;
                    }
                    continue;
                }
                if (s[i] == ' ') {
                    ++i;
                } else if (s[i] == '\n') {
                    ++i;
                } else if (s[i] == ',') {
                    ++i;
                    ifgetkey = true;
                } else if (s[i] == ':') {
                    ++i; 
                    ifgetvalue = true;
                } else if (s[i] == '}') {
                    ++i;
                    BracketStack.pop();
                    NodeStack.pop();
                    if (NodeStack.empty()) {
                        curNode = nullptr;
                    } else {
                        curNode = NodeStack.top();
                    }
                } else if (s[i] == '\t') {
                    ++i;
                }
            } else if (BracketStack.top() == ']') {//在Array里添加数据
                if (ifgetvalue) {
                    auto p = getNextValue(i,s);
                    ifgetvalue = false;
                    if (p != nullptr) {
                        curArray->push_back(p);
                        jsonType _tmptype = getValueType(p);
                        if (_tmptype == _OBJECT) {
                            curNode = NodeStack.top();
                            ifgetkey = true;
                        } else if (_tmptype == _ARRAY) {
                            curArray = ArrayStack.top();
                            ifgetvalue = true;
                        }
                    }
                    continue;
                }
                if (s[i] == ' ') {
                    ++i;
                } else if (s[i] == '\n') {
                    ++i;
                } else if (s[i] == ',') {
                    ++i;
                    ifgetvalue = true;
                } else if (s[i] == ']') {
                    ++i;
                    BracketStack.pop();
                    ArrayStack.pop();
                    if (ArrayStack.empty()) {
                        curArray = nullptr;
                    } else {
                        curArray = ArrayStack.top();
                    }
                } else if (s[i] == '\t') {
                    ++i;
                }
            }
        }
    }
    //     if (rowjson[i] == ' ') {
    //         ++i;
    //     } else if (rowjson[i] == '\n') {
    //         ++i;
    //     } else if (rowjson[i] == '{') {
    //         curNode = getObject(i);
    //         if (root == nullptr) {
    //             root = curNode;
    //         }
    //         curKey = getNextKey(i);
    //     } else if (rowjson[i] == ':') {
    //         ++i;
    //         jsonValue *p = getNextValue(i);
    //         curNode->map[curKey] = p;
    //         if (std::holds_alternative<jsonNode*>(p->value)) {
    //             curNode = NodeStack.top();
    //             curKey = getNextKey(i);
    //         } else if (std::holds_alternative<std::vector<Json::jsonValue*>*>(p->value)) {

    //         }
    //     } else if (rowjson[i] == ',') {
    //         ++i;
    //         curKey = getNextKey(i);
    //     } else if (rowjson[i] == '}') {
    //         ++i;
    //         BracketStack.pop();
    //         NodeStack.pop();
    //         if (NodeStack.empty()) {
    //             curNode = nullptr;
    //         } else {
    //             curNode = NodeStack.top();
    //         }
    //     }
    // }
    if (!BracketStack.empty()) {
        std::cout << "Invalid json file. (More '{')" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void Json::printBool(ptr_jsonValue value) {
    bool tmp = std::get<bool>(value->value);
    if (tmp) {
        std::cout << "true";
    } else {
        std::cout << "false";
    }
}

void Json::printDouble(ptr_jsonValue value) {
    double tmp = std::get<double>(value->value);
    std::cout << tmp;
}

void Json::printString(ptr_jsonValue value) {
    std::string tmp = std::get<std::string>(value->value);
    std::cout << '"' << tmp << '"';
}

void Json::printNull(ptr_jsonValue value) {
    std::nullptr_t tmp = std::get<std::nullptr_t>(value->value);
    std::cout << "null";
}

void Json::printArray(ptr_jsonValue value, int numoftab) {
    auto p = std::get<ptr_jsonArray>(value->value);
    int n = p->size();
    std::cout << "[\n";
    if (n == 0) {//节点为空
        for (int i = 0; i < numoftab; ++i) {
            std::cout << '\t';
        }
        std::cout << ']';
        return;     
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= numoftab; ++j) {
            std::cout << '\t';
        }
        auto tmp = p.get()->at(i);
        jsonType t = getValueType(tmp);
        std::string typeName = std::visit(VariantTypeGetter{}, tmp->value);
        if (t == _NULL) {
            printNull(tmp);
        } else if (t == _BOOLEAN) {
            printBool(tmp);
        } else if (t == _NUMBER) {
            printDouble(tmp);
        } else if (t == _STRING) {
            printString(tmp);
        } else if (t == _OBJECT) {
            auto nextnode = std::get<ptr_jsonNode>(tmp->value);
            printNode(nextnode, numoftab+1);
        } else if (t == _ARRAY) {
            printArray(tmp, numoftab+1);
        } else {
            std::cout << "The type of variant is: " << typeName << std::endl;
        }
        // std::cout << "The type of variant is: " << typeName << std::endl;
        if (i != n-1) {
            std::cout << ',';
        }
        std::cout << '\n';
    }
    for (int i = 0; i < numoftab; ++i) {
        std::cout << '\t';
    }
    std::cout << ']';
}

void Json::printNode(ptr_jsonNode node, int numoftab) {
    std::cout << "{\n";
    int nums = node->map.size();
    if (nums == 0) {//节点为空
        for (int i = 0; i < numoftab; ++i) {
            std::cout << '\t';
        }
        std::cout << '}';
        return;     
    }
    for (auto u : node->list) {
        for (int i = 0; i <= numoftab; ++i) {
            std::cout << '\t';
        }
        std::cout << '"' << u << "\": ";
        std::string typeName = std::visit(VariantTypeGetter{}, node->map[u]->value);
        jsonType t = getValueType(node->map[u]);
        if (t == _NULL) {
            printNull(node->map[u]);
        } else if (t == _BOOLEAN) {
            printBool(node->map[u]);
        } else if (t == _NUMBER) {
            printDouble(node->map[u]);
        } else if (t == _STRING) {
            printString(node->map[u]);
        } else if (t == _OBJECT) {
            auto p = std::get<ptr_jsonNode>(node->map[u]->value);
            printNode(p, numoftab+1);
        } else if (t == _ARRAY) {
            printArray(node->map[u], numoftab+1);
        } else {
            std::cout << "The type of variant is: " << typeName << std::endl;
        }
        // std::cout << "The type of variant is: " << typeName << std::endl;
        if (u != node->list.back()) {
            std::cout << ',';
        }
        std::cout << '\n';
    }
    for (int i = 0; i < numoftab; ++i) {
        std::cout << '\t';
    }
    std::cout << '}';
}

//用来打印数组里的值情况，因为没有map记录类型所以需要自己判断。
void Json::printValue(ptr_jsonValue value) {
    std::string typeName = std::visit(VariantTypeGetter{}, value->value);
    jsonType t = getValueType(value);
    if (t == _NULL) {
        printNull(value);
    } else if (t == _BOOLEAN) {
        printBool(value);
    } else if (t == _NUMBER) {
        printDouble(value);
    } else if (t == _STRING) {
        printString(value);
    } else if (t == _OBJECT) {
        auto p = std::get<ptr_jsonNode>(value->value);
        printNode(p, 0);
    } else if (t == _ARRAY) {
        printArray(value, 0);
    } else {
        std::cout << "The type of variant is: " << typeName << std::endl;
    }
}

//打印结点内的值情况，有typemap，传入类型参数。
void Json::printValue(ptr_jsonValue value, jsonType t) {
    if (t == _NULL) {
        printNull(value);
    } else if (t == _NUMBER) {
        printDouble(value);
    } else if (t == _BOOLEAN) {
        printBool(value);
    } else if (t == _STRING) {
        printString(value);
    } else if (t == _OBJECT) {
        printNode(std::get<ptr_jsonNode>(value->value),0);
    } else if (t == _ARRAY) {
        printArray(value, 0);
    } else {
        std::cout << "wrong type";
    }
}

void Json::PrintJson() {
    if (root == nullptr) {
        return;
    }
    printNode(root,0);
    std::cout << '\n';
}

std::string Json::SaveAsString() {
    std::string tmp = "";
    if (root == nullptr) {
        return tmp;
    }
    tmp = NodetoString(root, 0);
    return tmp;
}

std::string Json::NodetoString(ptr_jsonNode node, int numoftab) {
    std::string tmp = "";
    tmp += "{\n";
    int nums = node->map.size();
    if (nums == 0) {//节点为空
        for (int i = 0; i < numoftab; ++i) {
            tmp += '\t';
        }
        tmp += '}';
        return tmp;     
    }
    for (auto u : node->list) {
        for (int i = 0; i <= numoftab; ++i) {
            tmp += '\t';
        }
        tmp = tmp + '"' + u + "\": ";
        std::string typeName = std::visit(VariantTypeGetter{}, node->map[u]->value);
        jsonType t = getValueType(node->map[u]);
        if (t == _NULL) {
            tmp += NulltoString(node->map[u]);
        } else if (t == _BOOLEAN) {
            tmp += BooltoString(node->map[u]);
        } else if (t == _NUMBER) {
            tmp += DoubletoString(node->map[u]);
        } else if (t == _STRING) {
            tmp += StringtoString(node->map[u]);;
        } else if (t == _OBJECT) {
            auto p = std::get<ptr_jsonNode>(node->map[u]->value);
            tmp += NodetoString(p, numoftab+1);
        } else if (t == _ARRAY) {
            tmp += ArraytoString(node->map[u], numoftab+1);
        } else {
            tmp = tmp + "The type of variant is: " + typeName + '\n';
        }
        if (u != node->list.back()) {
            tmp += ',';
        }
        tmp += '\n';
    }
    for (int i = 0; i < numoftab; ++i) {
        tmp += '\t';
    }
    tmp += '}';
    return tmp;   
}

std::string Json::BooltoString(ptr_jsonValue value) {
    bool tmp = std::get<bool>(value->value);
    std::string s = "";
    if (tmp) {
        s += "true";
    } else {
        s += "false";
    }
    return s;
}

std::string Json::DoubletoString(ptr_jsonValue value) {
    double tmp = std::get<double>(value->value);
    std::string s = "";
    s += std::to_string(tmp);
    return s;
}

std::string Json::StringtoString(ptr_jsonValue value) {
    std::string tmp = std::get<std::string>(value->value);
    tmp =  '"' + tmp + '"';
    return tmp;
}

std::string Json::NulltoString(ptr_jsonValue value) {
    std::nullptr_t tmp = std::get<std::nullptr_t>(value->value);
    std::string s = "";
    s += "null";
    return s;
}

std::string Json::ArraytoString(ptr_jsonValue value, int numoftab) {
    auto p = std::get<ptr_jsonArray>(value->value);
    int n = p->size();
    std::string s = "";
    s += "[\n";
    if (n == 0) {//节点为空
        for (int i = 0; i < numoftab; ++i) {
            s += '\t';
        }
        s += ']';
        return s;     
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= numoftab; ++j) {
            s += '\t';
        }
        auto tmp = p.get()->at(i);
        std::string typeName = std::visit(VariantTypeGetter{}, tmp->value);
        jsonType t = getValueType(tmp);
        if (t == _NULL) {
            s += NulltoString(tmp);
        } else if (t == _BOOLEAN) {
            s += BooltoString(tmp);
        } else if (t == _NUMBER) {
            s += DoubletoString(tmp);
        } else if (t == _STRING) {
            s += StringtoString(tmp);
        } else if (t == _OBJECT) {
            auto nextnode = std::get<ptr_jsonNode>(tmp->value);
            s += NodetoString(nextnode, numoftab+1);
        } else if (t == _ARRAY) {
            s += ArraytoString(tmp, numoftab+1);
        } else {
            s = s + "The type of variant is: " + typeName + '\n';
        }
        if (i != n-1) {
            s += ',';
        }
        s += '\n';
    }
    for (int i = 0; i < numoftab; ++i) {
        s += '\t';
    }
    s += ']';
    return s;
}

void Json::SaveFile(std::string filename) {
    std::string completeString = SaveAsString();
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << completeString;
        outFile.close();
    }
}

//显示Json数据中所有的Key值。递归查找，并且显示类型。
void Json::ShowKeys() {
    if (root == nullptr) {
        std::cout << "no key";
    }
    ShowNodeKeys(root, 0, "root");
}

//把enum用string的形式显示
std::string Json::ShowJsonType(jsonType type) {
    std::string tmp = "";
    if (type == _NULL) {
        tmp = "NULL";
    } else if (type == _NUMBER) {
        tmp = "NUMBER";
    } else if (type == _BOOLEAN) {
        tmp = "BOOL";
    } else if (type == _STRING) {
        tmp = "STRING";
    } else if (type == _OBJECT) {
        tmp = "OBJECT";
    } else if (type == _ARRAY) {
        tmp = "ARRAY";
    } else {
        std::cout << "wrong type";
    }
    return tmp;
}

//显示Node结点中的所有Key值，需要输入参数和结点的名称，便于递归
void Json::ShowNodeKeys(ptr_jsonNode node, int numoftab, std::string name) {
    for (auto u : node->list) {
        for (int i = 0; i < numoftab; ++i) {
            std::cout << '\t';
        }
        std::cout << name << '.' << u << ": " ;
        std::cout << ShowJsonType(node->typemap[u]) << '\n';

        if (node->typemap[u] == _OBJECT) {
            auto nextnode = std::get<ptr_jsonNode>(node->map[u]->value);
            ShowNodeKeys(nextnode, numoftab+1, name+'.'+u);
        } else if (node->typemap[u] == _ARRAY) {
            auto nextarray = std::get<ptr_jsonArray>(node->map[u]->value);
            ShowArrayKeys(nextarray, numoftab+1, name+'.'+u);
        }
    }
}

//显示Array中所有key值，实际是遍历Array，如果发现有Object，就调用ShowNodeKeys()
void Json::ShowArrayKeys(ptr_jsonArray nextarray, int numoftab, std::string name) {
    int n = nextarray->size();
    for (int i = 0; i < n; ++i) {
        auto tmp = nextarray.get()->at(i);
        std::string typeName = std::visit(VariantTypeGetter{}, tmp->value);
        jsonType t = getValueType(tmp);
        if (t == _OBJECT) {
            auto nextnode = std::get<ptr_jsonNode>(tmp->value);
            ShowNodeKeys(nextnode, numoftab, name+'['+std::to_string(i)+']');
        } else if (t == _ARRAY) {
            auto nextarray = std::get<ptr_jsonArray>(tmp->value);
            ShowArrayKeys(nextarray, numoftab, name+'['+std::to_string(i)+']');
        }
    }
}

// 删除key,需要输入完整的路径名字 root.publisher.Country
void Json::DeleteKey(std::string Keyname) {
    std::string head = Keyname.substr(0,4);
    if (head != "root") {
        std::cout << "wrong keyname\n";
        return;
    }
    int n = head.size();
    head = Keyname.substr(5, n-5);
    DeleteKeyInNode(head, root);
}

// 删除结点内的key，如果是中间层，还需要迭代删除。
void Json::DeleteKeyInNode(std::string Keyname, ptr_jsonNode p){
    std::string name = "";
    int i = 0, n = Keyname.size();
    while (i < n && Keyname[i] != '.' && Keyname[i] != '[') {
        ++i;
    }
    name = Keyname.substr(0,i);
    if (!p->map.count(name)) {
        std::cout << "no keyname\n";
        return;
    }
    if (i == n) {
        p->map.erase(name);
        p->typemap.erase(name);
        p->list.remove(name);
    } else if (Keyname[i] == '.') {
        if (p->typemap[name] != _OBJECT) {
            std::cout << "keynamewrong,no such object\n";
            return;
        }
        auto nextnode = std::get<ptr_jsonNode>(p->map[name]->value);
        name = Keyname.substr(i+1,n-i-1);
        DeleteKeyInNode(name, nextnode);
    } else if (Keyname[i] == '[') {
        ++i;
        int j = i;
        while (Keyname[i] != ']') {
            ++i;
        }
        int sub = std::stoi(Keyname.substr(j,i-j));
        auto nextarray = std::get<ptr_jsonArray>(p->map[name]->value);
        if (i == n-1) {//抵达末尾，删除数组中某一个元素
            nextarray->erase(nextarray->begin()+sub);
        } else {
            auto nextnode = std::get<ptr_jsonNode>(nextarray.get()->at(i)->value);
            name = Keyname.substr(i+2,n-i-2);
            DeleteKeyInNode(name, nextnode);            
        }
    } else {
        std::cout << "wrong keyname\n";
        return;
    }
}

//删除结点值，根据它的类型删除。
// void Json::DeleteJsonValue(jsonValue *v, jsonType t) {
//     if (t == _ARRAY) {
//         std::vector<jsonValue*>* array = std::get<std::vector<jsonValue*>*>(v->value);
//         int n = array->size();
//         for (int i = 0; i < n; ++i) {
//             DeleteJsonValueInArray(array[0][i]);
//         }
//         array[0].erase(array[0].begin(),array[0].end());
//         delete array;
//     } else if (t == _OBJECT) {

//     }

//     v->value = nullptr;
//     delete v;
// }

// 查找Key对应的值，并且将对应的value输出
void Json::SearchKey(std::string Keyname) {
    std::string head = Keyname.substr(0,4);
    if (head != "root") {
        std::cout << "wrong keyname\n";
        return;
    }
    int n = head.size();
    head = Keyname.substr(5, n-5);
    std::cout << Keyname << ": ";
    SearchKeyInNode(head, root);
    std::cout << '\n';
}

void Json::SearchKeyInNode(std::string Keyname, ptr_jsonNode p) {
    std::string name = "";
    int i = 0, n = Keyname.size();
    while (i < n && Keyname[i] != '.' && Keyname[i] != '[') {
        ++i;
    }
    name = Keyname.substr(0,i);
    if (!p->map.count(name)) {
        std::cout << "no keyname\n";
        return;
    }
    if (i == n) {
        printValue(p->map[name],p->typemap[name]);
    } else if (Keyname[i] == '.') {
        if (p->typemap[name] != _OBJECT) {
            std::cout << "keynamewrong,no such object\n";
            return;
        }
        auto nextnode = std::get<ptr_jsonNode>(p->map[name]->value);
        name = Keyname.substr(i+1,n-i-1);
        SearchKeyInNode(name, nextnode);
    } else if (Keyname[i] == '[') {
        ++i;
        int j = i;
        while (Keyname[i] != ']') {
            ++i;
        }
        int sub = std::stoi(Keyname.substr(j,i-j));
        auto nextarray = std::get<ptr_jsonArray>(p->map[name]->value);
        if (i == n-1) {//抵达末尾。需要输出ARRAY中的某一个值
            printValue(nextarray.get()->at(sub));
        } else {
            auto nextnode = std::get<ptr_jsonNode>(nextarray.get()->at(sub)->value);
            name = Keyname.substr(i+2,n-i-2);
            SearchKeyInNode(name, nextnode);    
        }
    } else {
        std::cout << "wrong keyname\n";
        return;
    }
}

void Json::InsertKey(std::string Keyname, std::string value) {
    std::string head = Keyname.substr(0,4);
    if (head != "root") {
        std::cout << "wrong keyname\n";
        return;
    }
    int n = head.size();
    head = Keyname.substr(5, n-5);
    InsertKeyInNode(head, value, root);
}

void Json::InsertKeyInNode(std::string Keyname, std::string value, ptr_jsonNode p) {
    std::string name = "";
    // ptr_jsonValue next;
    // auto tmp = std::make_shared<std::vector<ptr_jsonValue>>();
    // ptr_jsonValue origin;
    int i = 0, n = Keyname.size();
    while (i < n && Keyname[i] != '.' && Keyname[i] != '[') {
        ++i;
    }
    name = Keyname.substr(0,i);
    if (i != n && !p->map.count(name)) {
        std::cout << "no keyname\n";
        return;
    }
    if (i == n) {
        auto next = StringtoValue(value);
        if (p->map.count(name)) {//已有key,插入value后转换为数组
            //如果原有的value也是数组，那么会变成嵌套二维数组。
            //这么做是为了和直接插入到数组下标处区分。
            // std::vector<jsonValue*> tmp;
            auto tmp = std::make_shared<std::vector<ptr_jsonValue>>();
            // auto origin = p->map[name];
            tmp.get()->push_back(p->map[name]);
            tmp.get()->push_back(next);
            // tmp.push_back(origin);
            // tmp.push_back(next);
            // delete p->map[name];
            p->map[name] = std::make_shared<jsonValue>(tmp);
            // std::vector<jsonValue*>* q = std::get<std::vector<jsonValue*>*>(p->map[name]->value);
            // printValue(q->at(0));
            // printValue(q->at(1));
            // p->map[name]->value = &tmp;
            // printValue(origin);
            // printValue(p->map[name]);
            // std::cout << '1';
        } else {
            p->map[name] = next;
        }
    } else if (Keyname[i] == '.') {
        if (p->typemap[name] != _OBJECT) {
            std::cout << "keynamewrong,no such object\n";
            return;
        }
        auto nextnode = std::get<ptr_jsonNode>(p->map[name]->value);
        name = Keyname.substr(i+1,n-i-1);
        InsertKeyInNode(name, value, nextnode);
    } else if (Keyname[i] == '[') {
        ++i;
        int j = i;
        while (Keyname[i] != ']') {
            ++i;
        }
        int sub = std::stoi(Keyname.substr(j,i-j));
        auto nextarray = std::get<ptr_jsonArray>(p->map[name]->value);
        if (i == n-1) {//抵达末尾。插入到ARRAY中的下标。
            auto next = StringtoValue(value);
            nextarray.get()->insert(nextarray.get()->begin()+sub, next);
            // nextarray[0].insert(nextarray[0].begin()+sub, next);
        } else {
            auto nextnode = std::get<ptr_jsonNode>(nextarray.get()->at(sub)->value);
            name = Keyname.substr(i+2,n-i-2);
            InsertKeyInNode(name, value, nextnode);    
        }
    } else {
        std::cout << "wrong keyname\n";
        return;
    }
    // std::vector<jsonValue*>* q = std::get<std::vector<jsonValue*>*>(p->map[name]->value);
    // printValue(q->at(0));
    // printValue(q->at(1));
}

Json::ptr_jsonValue Json::StringtoValue(std::string s) {
    int i = 0;
    if (s == "") {
        s = rowjson;
    }
    int n = s.size();
    auto returnvalue = getNextValue(i,s);
    bool ifgetkey, ifgetvalue;//用于表示当前是读key还是读value
    if (BracketStack.empty()) {//非Object，非Array
        return returnvalue;
    } else if (BracketStack.top() == '}') {
        ifgetkey = true;
        ifgetvalue = false;
        curNode = NodeStack.top();
    } else if (BracketStack.top() == ']') {
        ifgetvalue = true;
        curArray = ArrayStack.top();
    }
    while (i < n && !BracketStack.empty()) {
        if (BracketStack.top() == '}') {//在Node里添加数据
            if (ifgetkey) {
                curKey = getNextKey(i,s);
                if (curKey == "") {
                    ++i;
                    BracketStack.pop();
                    NodeStack.pop();
                    if (NodeStack.empty()) {
                        curNode = nullptr;
                    } else {
                        curNode = NodeStack.top();
                    }
                    ifgetkey = false;
                    continue;
                }
                curNode->list.push_back(curKey);
                ifgetkey = false;
            }
            if (ifgetvalue) {
                auto p = getNextValue(i,s);
                curNode->map[curKey] = p;
                jsonType _tmptype = getValueType(p);
                curNode->typemap[curKey] = _tmptype;
                ifgetvalue = false;
                if (_tmptype == _OBJECT) {
                    curNode = NodeStack.top();
                    ifgetkey = true;
                } else if (_tmptype == _ARRAY) {
                    curArray = ArrayStack.top();
                    ifgetvalue = true;
                }
                continue;
            }
            if (s[i] == ' ') {
                ++i;
            } else if (s[i] == '\n') {
                ++i;
            } else if (s[i] == ',') {
                ++i;
                ifgetkey = true;
            } else if (s[i] == ':') {
                ++i; 
                ifgetvalue = true;
            } else if (s[i] == '}') {
                ++i;
                BracketStack.pop();
                NodeStack.pop();
                if (NodeStack.empty()) {
                    curNode = nullptr;
                } else {
                    curNode = NodeStack.top();
                }
            } else if (s[i] == '\t') {
                ++i;
            }
        } else if (BracketStack.top() == ']') {//在Array里添加数据
            if (ifgetvalue) {
                auto p = getNextValue(i,s);
                ifgetvalue = false;
                if (p != nullptr) {
                    curArray->push_back(p);
                    jsonType _tmptype = getValueType(p);
                    if (_tmptype == _OBJECT) {
                        curNode = NodeStack.top();
                        ifgetkey = true;
                    } else if (_tmptype == _ARRAY) {
                        curArray = ArrayStack.top();
                        ifgetvalue = true;
                    }
                }
                continue;
            }
            if (s[i] == ' ') {
                ++i;
            } else if (s[i] == '\n') {
                ++i;
            } else if (s[i] == ',') {
                ++i;
                ifgetvalue = true;
            } else if (s[i] == ']') {
                ++i;
                BracketStack.pop();
                ArrayStack.pop();
                if (ArrayStack.empty()) {
                    curArray = nullptr;
                } else {
                    curArray = ArrayStack.top();
                }
            } else if (s[i] == '\t') {
                ++i;
            }
        }
    }
    if (!BracketStack.empty()) {
        std::cout << "Invalid json file. (More '{')" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return returnvalue;
}

}