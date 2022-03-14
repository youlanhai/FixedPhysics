#include "TestTool.hpp"
#include "math/FMath.hpp"

#include <cstdio>

NS_FXP_BEGIN

FXP_API const char* testCategory = "test";

FXP_API int ls_totalTest = 0;
FXP_API int ls_failedTest = 0;

template <typename T>
static bool almostEqual(T a, T b, T epsilon)
{
    T v = a - b;
    return v > -epsilon && v < epsilon;
}

FXP_API bool doTest(bool condition, const char *desc, const char *file, int line)
{
    ++ls_totalTest;
    if(!condition)
    {
        ++ls_failedTest;
        LOG_ERROR("Test Failed: [%s], %s, file: %s, line: %d", testCategory, desc, file, line);
    }
    return condition;
}

bool doTestCmp(const float &v, const float &expected, const char *file, int line)
{
    ++ls_totalTest;
    if (!almostEqual(v, expected, 0.0001f))
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

bool doTestCmp(const double &v, const double &expected, const char *file, int line)
{
    ++ls_totalTest;
    if (!almostEqual(v, expected, 0.0001))
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

FXP_API void reportTest()
{
    LOG_INFO("----------------------cpp test report----------------------");
    LOG_INFO("total test cases: %d", ls_totalTest);
    LOG_INFO("failed cases: %d", ls_failedTest);
    LOG_INFO("failed ratio: %0.3f", ls_failedTest * 100.0f / ls_totalTest);
    LOG_INFO("-----------------------------------------------------------");
}

NS_FXP_END
