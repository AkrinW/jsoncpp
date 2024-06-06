#ifndef TESTFRAME_H
#define TESTFRAME_H

#include <string>
#include <iostream>

enum TestReturn {
    PARSE_OK = 0,
    PARSE_EXPECT_VALUE,
    PARSE_INVALID_VALUE,
    PARSE_ROOT_NOT_SINGULAR
};

class TestFramework {
private:
    int main_ret;//测试的返回值，如果全部通过，返回0
    int test_count;//测试的次数
    int test_pass;//测试通过的次数
public:
    TestFramework(): main_ret(0), test_count(0), test_pass(0){};
    void expect_eq(int expect, int actual,std::string filename, int line) {
        test_count++;
        if (expect == actual) {
            test_pass++;
        } else {
            // 错误输出函数，无缓冲
            std::cerr << filename << ":" << line << ": expect: " << expect << " actual: " << actual << std::endl;
            main_ret = 1;
        }
    }
    void report() const {
        std::cout << test_pass << "/" << test_count << " (" 
                  << (test_pass * 100.0 / test_count) << "%) passed" << std::endl;
    }
    int result() const {
        return main_ret;
    }
};
//宏定义，简化代码
//需要把__FILE__, __LINE__写在这里，目的是作为参数传入，这样能够输出调用函数的位置
//如果写在函数定义里面，就只会输出函数定义的位置。
#define EXPECT_EQ(framework, expect, actual) (framework).expect_eq((expect), (actual), __FILE__, __LINE__)

#endif