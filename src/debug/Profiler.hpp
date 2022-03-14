//////////////////////////////////////////////////////////////////////
/// Desc  Profiler
/// Time  2021/09/28
/// Autor youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "ProfilerKeys.hpp"

#include <string>

NS_FXP_BEGIN

class ProfilerNode;
class ProfilerImp;

/** 性能分析统计类 */
class FXP_API Profiler
{
    DISABLE_COPY_AND_ASSIGN(Profiler);
public:
    Profiler();
    ~Profiler();

    void begin(int key);
    void end(int key = 0);

    /** 结束所有没有结束的结点 */
    void endAll();

    /** 清空所有数据 */
    void clear();

    /** 打印到控制台 */
    std::string report();

    /** @private */
    ProfilerNode* getRoot() { return root_; }

    void setEnable(bool enable);
    bool isEnabled() const { return enabled_; }

    void setCommaEnable(bool enable) { comma_ = enable; }

    static Profiler* getDefault();

private:
    ProfilerNode*   root_;
    ProfilerImp*    imp_;
    bool            enabled_;
    bool            comma_;
};

class FXP_API ProfilerScop
{
    int key_;
public:
    ProfilerScop(int key)
        : key_(key)
    {
        Profiler::getDefault()->begin(key);
    }

    ~ProfilerScop()
    {
        Profiler::getDefault()->end(key_);
    }
};

#ifdef ENABLE_TEST
#define LS_PROFILER_BEGIN(KEY)  Profiler::getDefault()->begin(KEY)
#define LS_PROFILER_END(KEY)    Profiler::getDefault()->end(KEY)
#define LS_PROFILER(KEY)        ProfilerScop ps_##KEY (KEY)
#else
#define LS_PROFILER_BEGIN(KEY)
#define LS_PROFILER_END(KEY)
#define LS_PROFILER(KEY)
#endif

NS_FXP_END
