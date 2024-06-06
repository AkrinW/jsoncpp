#include "../include/jsoncpp.h"
#include <memory>
#include <iostream>
#include <fstream>

namespace JSON{

struct Json::jsonNode {
    jsonType type;
    std::string key;
    int value;
};

Json::Json() {
    rowjson = "";
}

Json::~Json() {

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

void Json::PrintRowjson() {
    std::cout << rowjson;
}

}