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

struct jsonNode {
    jsonType type;
    std::string key;
    int value;
};

class Json {
private:
    std::string rowjson;
    jsonNode root;
public:
    Json();
    ~Json();
    void ReadFile(std::string filename);
    void PrintRowjson();
};

} // namespace json

#endif