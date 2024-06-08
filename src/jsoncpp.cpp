#include "../include/jsoncpp.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <typeinfo>


namespace JSON{

//获取variant类型
struct VariantTypeGetter {
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
};

struct Json::jsonNode {
    std::unordered_map<std::string, jsonValue*> map;
    // std::unordered_map<std::string, jsonValue> map;
};

Json::Json() {
    rowjson = "";
    root = nullptr;
    curNode = nullptr;
    curKey = "";
}

Json::~Json() {
    delete root;
    delete curNode;
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
        }
    }
    return p;

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
                }
            }
        } else {
            if (BracketStack.top() == '}') {//在Node里添加数据
                if (ifgetkey) {
                    curKey = getNextKey(i);
                    ifgetkey = false;
                }
                if (ifgetvalue) {
                    jsonValue *p = getNextValue(i);
                    curNode->map[curKey] = p;
                    ifgetvalue = false;
                    if (std::holds_alternative<jsonNode*>(p->value)) {
                        curNode = NodeStack.top();
                        ifgetkey = true;
                    } else if (std::holds_alternative<std::vector<Json::jsonValue*>*>(p->value)) {
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
                }
            } else if (BracketStack.top() == ']') {//在Array里添加数据
                if (ifgetvalue) {
                    jsonValue *p = getNextValue(i);
                    curArray->push_back(p);
                    // curNode->map[curKey] = p;
                    ifgetvalue = false;
                    if (std::holds_alternative<jsonNode*>(p->value)) {
                        curNode = NodeStack.top();
                        ifgetkey = true;
                    } else if (std::holds_alternative<std::vector<Json::jsonValue*>*>(p->value)) {
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

void Json::printBool() {
    bool tmp = std::get<bool>(curNode->map[curKey]->value);
    std::cout << tmp;
}

void Json::printDouble() {
    double tmp = std::get<double>(curNode->map[curKey]->value);
    std::cout << tmp;
}

void Json::printString() {
    std::string tmp = std::get<std::string>(curNode->map[curKey]->value);
    std::cout << tmp;
}


}