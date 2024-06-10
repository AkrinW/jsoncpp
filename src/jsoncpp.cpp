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
//  const char* typeName = std::visit(VariantTypeGetter{}, curNode->map[curKey]);
//  std::cout << "The type of variant is: " << typeName << std::endl;

struct Json::jsonValue {
    std::variant<std::nullptr_t, std::string, bool, 
                double, jsonNode*, std::vector<jsonValue*>*> value;
    jsonValue() {}
    jsonValue(std::nullptr_t nptr): value(nptr){}
    jsonValue(bool flag): value(flag){}
    jsonValue(std::string s): value(s){}
    jsonValue(double d): value(d){}
    jsonValue(jsonNode* n): value(n){}
    jsonValue(std::vector<jsonValue*>* v): value(v) {}
    ~jsonValue();
};

struct Json::jsonNode {
    std::unordered_map<std::string, jsonValue*> map;
    std::list<std::string> list;
    std::unordered_map<std::string, jsonType> typemap;
    // std::unordered_map<std::string, jsonValue> map;
    ~jsonNode();
};

Json::jsonValue::~jsonValue() {
    if (std::holds_alternative<jsonNode*>(value)) {
        jsonNode *n = std::get<jsonNode*>(value);
        delete n;
    } else if (std::holds_alternative<std::vector<Json::jsonValue*>*>(value)) {
        std::vector<Json::jsonValue*>* v = std::get<std::vector<Json::jsonValue*>*>(value);
        int n = v->size();
        for (int i = 0; i < n; ++i) {
            delete v[0][i];
        }
        v->clear();
        delete v;
    } else if (std::holds_alternative<std::nullptr_t>(value)) {

    } else if (std::holds_alternative<double>(value)) {

    } else if (std::holds_alternative<std::string>(value)) {

    } else if (std::holds_alternative<bool>(value)) {

    } else {
        std::cout << "wrong type";
    }
    value = nullptr;
}

Json::jsonNode::~jsonNode() {
    list.clear();
    typemap.clear();
    for (auto u : map) {
        delete u.second;
    }
    map.clear();
}

Json::Json() {
    rowjson = "";
    root = nullptr;
    curNode = nullptr;
    curKey = "";
}

Json::~Json() {
    delete root;
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
    for (auto u : list) {
        std::cout << u << ' ' << map[u] << std::endl;
    }
}

void Json::InitNode() {
    root = new jsonNode();
}

//no use now.
void Json::AddNode() {
    if (root == nullptr) {
        InitNode();
        curNode = root;
    } else {
        curNode->map[curKey] = new jsonValue(new jsonNode());
        // auto pval = std::get_if<jsonNode*>(&curNode->map[curKey].value);
        curNode = std::get<jsonNode*>(curNode->map[curKey]->value);
    }
}

//getsometing() only used by build() with rowjson.
std::string Json::getString(int &i) {
    std::string tmp = "";
    ++i;
    int j = i;
    while (rowjson[i] != '"') {
        if (rowjson[i] == '\\') {
            ++i;
            if (rowjson[i] == 'n') {
                tmp += '\n';
            } else if (rowjson[i] == 't') {
                tmp += '\t';
            } else if (rowjson[i] == '\'') {
                tmp += '\'';
            } else if (rowjson[i] == '\\') {
                tmp += '\\';
            } else if (rowjson[i] == '"') {
                std::cout << "Invalid json file. (wrong '\\' position)" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        } else {
            tmp += rowjson[i];
        }
        ++i;
    }
    ++i;
    return tmp;
}

double Json::getDouble(int &i) {
    std::string tmp = "";
    int j = i;
    while (rowjson[i] != ' ' && rowjson[i] != ',' 
            && rowjson[i] != '\n' && rowjson[i] != '}'
            && rowjson[i] != ']') {
       ++i;
    }
    tmp = rowjson.substr(j,i-j);
    return std::stod(tmp);
}

bool Json::getBool(int &i) {
    if (rowjson[i] == 'f') {
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

Json::jsonNode* Json::getObject(int &i) {
    jsonNode *p = new jsonNode();
    BracketStack.push('}');
    NodeStack.push(p);
    ++i;
    return p;
}

std::vector<Json::jsonValue*>* JSON::Json::getArray(int &i) {
    std::vector<Json::jsonValue*>* p = new std::vector<Json::jsonValue*>();
    BracketStack.push(']');
    ArrayStack.push(p);
    ++i;
    return p;
}

std::string Json::getNextKey(int &i) {
    std::string tmp = "";
    while (i < rown) {
        if (rowjson[i] == ' ') {
            ++i;
        } else if (rowjson[i] == '\n') {
            ++i;
        } else if (rowjson[i] == ',') {
            ++i;
        } else if (rowjson[i] == '"') {
            tmp = getString(i);
            break;
        } else if (rowjson[i] == '}') {
            break;
        } else if (rowjson[i] == '\t') {
            ++i;
        }
    }
    return tmp;
}

Json::jsonValue* Json::getNextValue(int &i) {
    jsonValue *p;
    while (i < rown) {
        if (rowjson[i] == ' ') {
            ++i;
        } else if (rowjson[i] == '\n') {
            ++i;
        } else if (rowjson[i] == ':') {
            ++i;
        } else if (rowjson[i] == '"') {
            p = new jsonValue(getString(i));
            break;
        } else if (rowjson[i] == 't' || rowjson[i] == 'f') {
            p = new jsonValue(getBool(i));
            break;
        } else if (rowjson[i] == 'n') {
            p = new jsonValue(getNull(i));
            if (p == nullptr) {
                std::cout <<"jsonValue(null) == nullptr";
            }
            break;
        } else if (rowjson[i] == '{') {
            p = new jsonValue(getObject(i));
            break;
        } else if (rowjson[i] == '-' || (rowjson[i] >= '0' && rowjson[i] <= '9')) {
            p = new jsonValue(getDouble(i));
            break;
        } else if (rowjson[i] == '[') {
            p = new jsonValue(getArray(i));
            break;
        } else if (rowjson[i] == ']') {
            p = nullptr;
            break;
        } else if (rowjson[i] == '\t') {
            ++i;
        }
    }
    return p;

}

jsonType Json::getValueType(jsonValue *p) {
    jsonType tmp;
    if (std::holds_alternative<jsonNode*>(p->value)) {
        tmp = _OBJECT;
    } else if (std::holds_alternative<std::vector<Json::jsonValue*>*>(p->value)) {
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

void Json::Build() {
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
    bool ifgetkey, ifgetvalue;//用于表示当前是读key还是读value
    //因为存在两个都不读的情况，所以需要设置2个。
    while (i < rown) {
        if (BracketStack.empty()) {
            while (BracketStack.empty() && i < rown) {
                if (rowjson[i] == ' ') {
                    ++i;
                } else if (rowjson[i] == '\n') {
                    ++i;
                } else if (rowjson[i] == '{') {
                    curNode = getObject(i);
                    root = curNode;
                    ifgetkey = true;
                    ifgetvalue = false;
                } else if (rowjson[i] == '\t') {
                    ++i;
                }
            }
        } else {
            if (BracketStack.top() == '}') {//在Node里添加数据
                if (ifgetkey) {
                    curKey = getNextKey(i);
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
                    jsonValue *p = getNextValue(i);
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
                if (rowjson[i] == ' ') {
                    ++i;
                } else if (rowjson[i] == '\n') {
                    ++i;
                } else if (rowjson[i] == ',') {
                    ++i;
                    ifgetkey = true;
                } else if (rowjson[i] == ':') {
                    ++i; 
                    ifgetvalue = true;
                } else if (rowjson[i] == '}') {
                    ++i;
                    BracketStack.pop();
                    NodeStack.pop();
                    if (NodeStack.empty()) {
                        curNode = nullptr;
                    } else {
                        curNode = NodeStack.top();
                    }
                } else if (rowjson[i] == '\t') {
                    ++i;
                }
            } else if (BracketStack.top() == ']') {//在Array里添加数据
                if (ifgetvalue) {
                    jsonValue *p = getNextValue(i);
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
                if (rowjson[i] == ' ') {
                    ++i;
                } else if (rowjson[i] == '\n') {
                    ++i;
                } else if (rowjson[i] == ',') {
                    ++i;
                    ifgetvalue = true;
                } else if (rowjson[i] == ']') {
                    ++i;
                    BracketStack.pop();
                    ArrayStack.pop();
                    if (ArrayStack.empty()) {
                        curArray = nullptr;
                    } else {
                        curArray = ArrayStack.top();
                    }
                } else if (rowjson[i] == '\t') {
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

void Json::printBool(jsonValue* value) {
    bool tmp = std::get<bool>(value->value);
    if (tmp) {
        std::cout << "true";
    } else {
        std::cout << "false";
    }
}

void Json::printDouble(jsonValue* value) {
    double tmp = std::get<double>(value->value);
    std::cout << tmp;
}

void Json::printString(jsonValue* value) {
    std::string tmp = std::get<std::string>(value->value);
    std::cout << '"' << tmp << '"';
}

void Json::printNull(jsonValue* value) {
    std::nullptr_t tmp = std::get<std::nullptr_t>(value->value);
    std::cout << "null";
}

void Json::printArray(jsonValue* value, int numoftab) {
    std::vector<jsonValue*>* p = std::get<std::vector<jsonValue*>*>(value->value);
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
        jsonValue* tmp = p[0][i];
        std::string typeName = std::visit(VariantTypeGetter{}, tmp->value);
        if (typeName == "Dn") {
            printNull(tmp);
        } else if (typeName == "b") {
            printBool(tmp);
        } else if (typeName == "d") {
            printDouble(tmp);
        } else if (typeName == "NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE") {
            printString(tmp);
        } else if (typeName == "PN4JSON4Json8jsonNodeE") {
            jsonNode *nextnode = std::get<jsonNode*>(tmp->value);
            printNode(nextnode, numoftab+1);
        } else if (typeName == "PSt6vectorIPN4JSON4Json9jsonValueESaIS3_EE") {
            printArray(tmp, numoftab+1);
        } else {
            std::cout << "The type of variant is: " << typeName << std::endl;
        }
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

void Json::printNode(jsonNode* node, int numoftab) {
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
        if (typeName == "Dn") {
            printNull(node->map[u]);
        } else if (typeName == "b") {
            printBool(node->map[u]);
        } else if (typeName == "d") {
            printDouble(node->map[u]);
        } else if (typeName == "NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE") {
            printString(node->map[u]);
        } else if (typeName == "PN4JSON4Json8jsonNodeE") {
            jsonNode *p = std::get<jsonNode*>(node->map[u]->value);
            printNode(p, numoftab+1);
        } else if (typeName == "PSt6vectorIPN4JSON4Json9jsonValueESaIS3_EE") {
            printArray(node->map[u], numoftab+1);
        } else {
            std::cout << "The type of variant is: " << typeName << std::endl;
        }
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

// void Json::printValue(jsonValue* value) {
//     const char* typeName = std::visit(VariantTypeGetter{}, value->value);
//     std::cout << "The type of variant is: " << typeName << std::endl;

// }

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

std::string Json::NodetoString(jsonNode* node, int numoftab) {
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
        if (typeName == "Dn") {
            tmp += NulltoString(node->map[u]);
        } else if (typeName == "b") {
            tmp += BooltoString(node->map[u]);
        } else if (typeName == "d") {
            tmp += DoubletoString(node->map[u]);
        } else if (typeName == "NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE") {
            tmp += StringtoString(node->map[u]);;
        } else if (typeName == "PN4JSON4Json8jsonNodeE") {
            jsonNode *p = std::get<jsonNode*>(node->map[u]->value);
            tmp += NodetoString(p, numoftab+1);
        } else if (typeName == "PSt6vectorIPN4JSON4Json9jsonValueESaIS3_EE") {
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

std::string Json::BooltoString(jsonValue* value) {
    bool tmp = std::get<bool>(value->value);
    std::string s = "";
    if (tmp) {
        s += "true";
    } else {
        s += "false";
    }
    return s;
}

std::string Json::DoubletoString(jsonValue* value) {
    double tmp = std::get<double>(value->value);
    std::string s = "";
    s += std::to_string(tmp);
    return s;
}

std::string Json::StringtoString(jsonValue* value) {
    std::string tmp = std::get<std::string>(value->value);
    tmp =  '"' + tmp + '"';
    return tmp;
}

std::string Json::NulltoString(jsonValue* value) {
    std::nullptr_t tmp = std::get<std::nullptr_t>(value->value);
    std::string s = "";
    s += "null";
    return s;
}

std::string Json::ArraytoString(jsonValue* value, int numoftab) {
    std::vector<jsonValue*>* p = std::get<std::vector<jsonValue*>*>(value->value);
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
        jsonValue* tmp = p[0][i];
        std::string typeName = std::visit(VariantTypeGetter{}, tmp->value);
        if (typeName == "Dn") {
            s += NulltoString(tmp);
        } else if (typeName == "b") {
            s += BooltoString(tmp);
        } else if (typeName == "d") {
            s += DoubletoString(tmp);
        } else if (typeName == "NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE") {
            s += StringtoString(tmp);
        } else if (typeName == "PN4JSON4Json8jsonNodeE") {
            jsonNode *nextnode = std::get<jsonNode*>(tmp->value);
            s += NodetoString(nextnode, numoftab+1);
        } else if (typeName == "PSt6vectorIPN4JSON4Json9jsonValueESaIS3_EE") {
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
void Json::ShowNodeKeys(jsonNode *node, int numoftab, std::string name) {
    for (auto u : node->list) {
        for (int i = 0; i < numoftab; ++i) {
            std::cout << '\t';
        }
        std::cout << name << '.' << u << ": " ;
        std::cout << ShowJsonType(node->typemap[u]) << '\n';

        if (node->typemap[u] == _OBJECT) {
            jsonNode *nextnode = std::get<jsonNode*>(node->map[u]->value);
            ShowNodeKeys(nextnode, numoftab+1, name+'.'+u);
        } else if (node->typemap[u] == _ARRAY) {
            std::vector<jsonValue*>* nextarray = std::get<std::vector<jsonValue*>*>(node->map[u]->value);
            ShowArrayKeys(nextarray, numoftab+1, name+'.'+u);
        }
    }
}

//显示Array中所有key值，实际是遍历Array，如果发现有Object，就调用ShowNodeKeys()
void Json::ShowArrayKeys(std::vector<jsonValue*>* nextarray, int numoftab, std::string name) {
    int n = nextarray->size();
    for (int i = 0; i < n; ++i) {
        jsonValue* tmp = nextarray[0][i];
        std::string typeName = std::visit(VariantTypeGetter{}, tmp->value);
        if (typeName == "PN4JSON4Json8jsonNodeE") {
            jsonNode *nextnode = std::get<jsonNode*>(tmp->value);
            ShowNodeKeys(nextnode, numoftab, name+'['+std::to_string(i)+']');
        } else if (typeName == "PSt6vectorIPN4JSON4Json9jsonValueESaIS3_EE") {
            std::vector<jsonValue*>* nextarray = std::get<std::vector<jsonValue*>*>(tmp->value);
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
void Json::DeleteKeyInNode(std::string Keyname, jsonNode *p) {
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
        delete p->map[name];
        p->map.erase(name);
        p->typemap.erase(name);
        p->list.remove(name);
    } else if (Keyname[i] == '.') {
        if (p->typemap[name] != _OBJECT) {
            std::cout << "keynamewrong,no such object\n";
            return;
        }
        jsonNode *nextnode = std::get<jsonNode*>(p->map[name]->value);
        name = Keyname.substr(i+1,n-i-1);
        DeleteKeyInNode(name, nextnode);
    } else if (Keyname[i] == '[') {
        ++i;
        int j = i;
        while (Keyname[i] != ']') {
            ++i;
        }
        int sub = std::stoi(Keyname.substr(j,i-j));
        std::vector<jsonValue*>* nextarray = std::get<std::vector<jsonValue*>*>(p->map[name]->value);
        jsonNode *nextnode = std::get<jsonNode*>(nextarray[0][i]->value);
        name = Keyname.substr(i+2,n-i-2);
        DeleteKeyInNode(name, nextnode);
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


}