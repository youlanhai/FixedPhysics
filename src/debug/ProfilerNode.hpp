//////////////////////////////////////////////////////////////////////
/// Desc  ProfilerNode
/// Time  2021/09/28
/// Autor youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "common/FConfig.hpp"
#include <unordered_map>
#include <algorithm>

NS_FXP_BEGIN

FXP_API uint64_t getHighPrecisionTimeUs();

class FXP_API ProfilerNode
{
    DISABLE_COPY_AND_ASSIGN(ProfilerNode);
public:
    explicit ProfilerNode(int key);
    ~ProfilerNode();

    void begin();
    void end();
    void clear();

    int getKey() const { return key_; }

    ProfilerNode* getChild(int key, bool createIfMissing);


    const std::unordered_map<int, ProfilerNode*>& getChildren() const { return children_; }

public:
    uint64_t count = 0;
    uint64_t totalCost = 0;
    uint64_t minCost = (size_t)-1;
    uint64_t maxCost = 0;

private:
    int key_ = 0;
    uint64_t startTime_ = 0;
    std::unordered_map<int, ProfilerNode*> children_;
};

inline ProfilerNode::ProfilerNode(int key)
    : key_(key)
{}

inline ProfilerNode::~ProfilerNode()
{
    clear();
}

inline void ProfilerNode::begin()
{
    startTime_ = getHighPrecisionTimeUs();
}

inline void ProfilerNode::end()
{
    ++count;
    uint64_t t = getHighPrecisionTimeUs() - startTime_;
    totalCost += t;
    minCost = std::min(minCost, t);
    maxCost = std::max(maxCost, t);
}

NS_FXP_END
