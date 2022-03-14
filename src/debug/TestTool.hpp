#pragma once
#include "LogTool.hpp"

#include <string>
#include <sstream>

NS_FXP_BEGIN

extern FXP_API const char* testCategory;

extern FXP_API int ls_totalTest;
extern FXP_API int ls_failedTest;

FXP_API bool doTest(bool condition, const char *desc, const char *file, int line);

template<typename T, typename U>
bool doTestCmp(const T &v, const U &expected, const char *file, int line)
{
    ++ls_totalTest;
    if(v != expected)
    {
        ++ls_failedTest;
        
        std::ostringstream os;
        os << '\'' << v << "' != '" << expected << '\'';
        std::string desc = os.str();
        LOG_ERROR("Test Failed: [%s], %s, file: %s, line: %d", testCategory, desc.c_str(), file, line);
        return false;
    }
    return true;
}

bool doTestCmp(const float &v, const float &expected, const char *file, int line);
bool doTestCmp(const double &v, const double &expected, const char *file, int line);

FXP_API void reportTest();

#define LS_BEGIN_TEST(name) testCategory = #name; LOG_INFO("-----------------begin test: %s", testCategory)
#define LS_TEST(a) doTest(a, #a, __FILE__, __LINE__)
#define LS_TEST_CMP(a, b) doTestCmp(a, b, __FILE__, __LINE__)
#define LS_TEST_DESC(a, desc) doTest(a, desc, __FILE__, __LINE__)
#define LS_END_TEST() testCategory = "test"

NS_FXP_END
