#include "../include/jsoncpp.h"
#include <memory>
#include <iostream>
#include <fstream>

namespace JSON{

struct Json::jsonNode {
    std::unordered_map<std::string, std::string> map;
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
        // curNode->map[curKey] = new jsonNode();
        // curNode = curNode->map[curKey];
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
        }
        ++i;
    }
    if (!BracketStack.empty()) {
        std::cout << "Invalid json file. (More '{')" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
}