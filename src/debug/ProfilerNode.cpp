//////////////////////////////////////////////////////////////////////
/// Desc  ProfilerNode
/// Time  2021/09/28
/// Autor youlanhai
//////////////////////////////////////////////////////////////////////

#include "ProfilerNode.hpp"

#if defined(_WIN32)
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif

NS_FXP_BEGIN

FXP_API uint64_t getHighPrecisionTimeUs()
{
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    uint64_t t = (uint64_t(ft.dwHighDateTime) << 32) | uint64_t(ft.dwLowDateTime); // 单位是100ns
    /* Windows file time (time since January 1, 1601 (UTC)) */
    /* convert to Unix Epoch time (time since January 1, 1970 (UTC)) */
    return (t / 10 - 11644473600000000LL);
#else
    struct timeval v;
    gettimeofday(&v, (struct timezone*)NULL);
    /* Unix Epoch time (time since January 1, 1970 (UTC)) */
    return uint64_t(v.tv_sec) * 1000000 + uint64_t(v.tv_usec);
#endif
}



void ProfilerNode::clear()
{
    for (auto &pair : children_)
    {
        delete pair.second;
    }
    children_.clear();
}

ProfilerNode* ProfilerNode::getChild(int key, bool createIfMissing)
{
    auto it = children_.find(key);
    if (it != children_.end())
    {
        return it->second;
    }

    if (createIfMissing)
    {
        ProfilerNode* p = new ProfilerNode(key);
        children_[key] = p;
        return p;
    }

    return nullptr;
}

NS_FXP_END
