#include "../include/jsoncpp.h"
#include <iostream>
#include <fstream>

namespace JSON{

Json::Json() {
    
}

Json::~Json() {

}

void Json::ReadFile(std::string filename) {
    std::ifstream file;
    file.open(filename);
    std::string line;
    while(file) {
        std::getline(file, line);
        rowjson.append(line);
    }
}

void Json::PrintRowjson() {
    std::cout << rowjson;
}

}