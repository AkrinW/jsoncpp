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


struct Json::jsonNode {
    std::unordered_map<std::string, Valuetype> map;
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
}

//读取json文件后，检查是否合法，保证后续储存的都是有效json
bool Json::IfValid(std::string json) {

}

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
        curNode->map[curKey] = new jsonNode();
        curNode = std::get<jsonNode*>(curNode->map[curKey]);
    }
}

void Json::Build() {
    int n = rowjson.size();
    int i = 0;
    bool readKey = false; //true-key, false-value;
    while (i < n) {
        if (rowjson[i] == ' ') {

        } else if (rowjson[i] == '\n') {

        } else if (rowjson[i] == ':') {
            readKey = false;
        } else if (rowjson[i] == ',') {
            readKey = true;
        } else if (rowjson[i] == '{') {
            if (readKey) {
                std::cout << "Invalid json file. (wrong '{}' position)" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            readKey = true;
            AddNode();
            BracketStack.push('}');
            NodeStack.push(curNode);
        } else if (rowjson[i] == '}') {
            if (BracketStack.empty() || BracketStack.top() != '}') {
                std::cout << "Invalid json file. (More '}')" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            BracketStack.pop();
            NodeStack.pop();
            if (NodeStack.empty()) {
                curNode = nullptr;
            } else {
                curNode = NodeStack.top();
            }
        } else if (rowjson[i] == '"') {
            ++i;
            std::string tmp = "";
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
            if (readKey) {
                curKey = tmp;
            } else {
                curNode->map[curKey] = tmp;
            }
        } else if (rowjson[i] == 'n') {
            i += 4;
            curNode->map[curKey] = std::nullptr_t();
        } else if (rowjson[i] == 'f') {
            i += 5;
            curNode->map[curKey] = false;
        } else if (rowjson[i] == 't') {
            i += 4;
            curNode->map[curKey] = true;
        } else if (rowjson[i] == '-' || (rowjson[i] >= '0' && rowjson[i] <= '9')) {
            std::string tmp = "";
            int j = i;
            while (rowjson[i] != ' ' && rowjson[i] != ',' 
            && rowjson[i] != '\n' && rowjson[i] != '}') {
                ++i;
            }
            tmp = rowjson.substr(j,i-j);
            --i;
            curNode->map[curKey] = std::stod(tmp);
        }
        ++i;
    }
    if (!BracketStack.empty()) {
        std::cout << "Invalid json file. (More '{')" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void Json::printBool() {
    bool tmp = std::get<bool>(curNode->map[curKey]);
    std::cout << tmp;
}

void Json::printDouble() {
    double tmp = std::get<double>(curNode->map[curKey]);
    std::cout << tmp;
}


}