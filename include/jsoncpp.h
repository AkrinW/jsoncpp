#ifndef JSONCPP_H
#define JSONCPP_H

#include <string>

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
    jsonNode *root;
    std::string rowjson;

public:
    Json();
    ~Json();
    void ReadFile(std::string filename);
    void PrintRowjson();
};

} // namespace json

#endif