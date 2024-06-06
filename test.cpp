#include "include/jsoncpp.h"
#include "include/testframe.h"

#include <string>
#include <cassert>

//设置测试值
struct TestValue {
    JSON::jsonType type;
};

//获取测试值类型
JSON::jsonType TestGetType(const TestValue* v) {
    return v->type;
}

//用于测试类型
int TestParse(TestValue* v, std::string json) {
    if (json == "null") {
        v->type = JSON::_NULL;
        return PARSE_OK;
    }
    return PARSE_EXPECT_VALUE; // 假设 1 表示解析失败
}

void TestParseNull(TestFramework& framework) {
    TestValue v;
    v.type = JSON::_BOOLEAN;
    EXPECT_EQ(framework, PARSE_OK, TestParse(&v, "null"));
    EXPECT_EQ(framework, JSON::_ARRAY, TestGetType(&v));
}

void TestParseBool(TestFramework& framework) {
    
}


void TestParse(TestFramework& framework) {
    TestParseNull(framework);
    // 添加更多测试...
}

int main() {

    TestFramework framework;
    TestParse(framework);
    framework.report();
    return framework.result();
}


// typedef struct {
//     const char* json;
// }lept_context;

// /* ... */

// /* 提示：这里应该是 JSON-text = ws value ws，*/
// /* 以下实现没处理最后的 ws 和 LEPT_PARSE_ROOT_NOT_SINGULAR */
// int lept_parse(lept_value* v, const char* json) {
//     lept_context c;
//     assert(v != NULL);
//     c.json = json;
//     v->type = LEPT_NULL;
//     lept_parse_whitespace(&c);
//     return lept_parse_value(&c, v);
// }