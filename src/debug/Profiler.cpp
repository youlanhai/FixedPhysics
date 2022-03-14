//////////////////////////////////////////////////////////////////////
/// Desc  Profiler
/// Time  2021/09/28
/// Autor youlanhai
//////////////////////////////////////////////////////////////////////

#include "Profiler.hpp"
#include "ProfilerNode.hpp"
#include "LogTool.hpp"

#include <stack>
#include <vector>
#include <sstream>
#include <algorithm>
#include <locale>
#include <iomanip>

NS_FXP_BEGIN

const std::string& getProfilerKeyName(int key);

// 用于隐藏stl容器，避免使用的地方引入太多头文件，影响编译效率
class ProfilerImp
{
public:
    std::stack<ProfilerNode*> stack;
    // std::mutex lock;
};

Profiler::Profiler()
    : enabled_(true)
    , comma_(true)
{
    root_ = new ProfilerNode(PK_ROOT);
    imp_ = new ProfilerImp();
}

Profiler::~Profiler()
{
    delete root_;
}

void Profiler::begin(int key)
{
    if (!enabled_)
    {
        return;
    }

    auto &stack = imp_->stack;

    if (stack.empty())
    {
        stack.push(root_);
        root_->begin();
    }

    ProfilerNode *top = stack.top();

    ProfilerNode *node = top->getChild(key, true);
    stack.push(node);

    node->begin();
}

void Profiler::end(int key)
{
    if (!enabled_)
    {
        return;
    }

    auto &stack = imp_->stack;

    if (stack.empty())
    {
        LOG_WARN("Profiler::end failed. stack is empty. key: %d", key);
        return;
    }

    if (key != 0 && stack.top()->getKey() != key)
    {
        LOG_WARN("Profiler::end failed. key(%d) is miss match wit top key(%d)", key, stack.top()->getKey());
        return;
    }

    ProfilerNode *node = stack.top();
    stack.pop();

    node->end();
}

void Profiler::endAll()
{
    while (!imp_->stack.empty())
    {
        end();
    }
}

void Profiler::clear()
{
    std::stack<ProfilerNode*> temp;
    imp_->stack.swap(temp);

    root_->clear();
}

void Profiler::setEnable(bool enable)
{
    if (enable == enabled_)
    {
        return;
    }

    enabled_ = enable;
    endAll();
}

static std::ostream& indent(std::ostream &stream, int depth)
{
    for (int i = 0; i < depth; ++i)
    {
        stream << "    ";
    }
    return stream;
}

struct Ms
{
    uint64_t v;

    Ms(uint64_t _v)
        : v (_v)
    {}
};


inline std::ostream& operator << (std::ostream &stream, Ms ms)
{
    stream << ms.v / 1000 << '.' << (ms.v % 1000) / 100 << (ms.v % 100) / 10 << (ms.v % 10);
    return stream;
}

static void reportNode(std::ostream &stream, const ProfilerNode &node, int depth)
{
    indent(stream, depth);

    const std::string& name = getProfilerKeyName(node.getKey());
    if (name.empty())
    {
        stream << "[" << node.getKey() << "]";
    }
    else
    {
        stream << name;
    }

    stream << " = {"
        << "count=" << node.count
        << ", total=" << Ms(node.totalCost)
        << ", min=" << Ms(node.minCost)
        << ", max=" << Ms(node.maxCost)
        << ", avg=" << Ms(node.count > 0 ? node.totalCost / node.count : 0);

    if (node.getChildren().size() > 0)
    {
        uint64_t childrenTotal = 0;
        std::vector<ProfilerNode*> children;
        children.reserve(node.getChildren().size());
        for (const auto &pair : node.getChildren())
        {
            children.push_back(pair.second);
            childrenTotal += pair.second->totalCost;
        }

        std::sort(children.begin(), children.end(), [] (ProfilerNode *a, ProfilerNode *b) {
            return a->totalCost > b->totalCost;
        });

        stream << ", self=" << Ms(node.totalCost - childrenTotal)
            << ", children = {" << std::endl;

        for (const ProfilerNode *node : children)
        {
            reportNode(stream, *node, depth + 1);
        }

        indent(stream, depth);
        stream << "}";
    }

    stream << "}," << std::endl;
}

std::string Profiler::report()
{
    std::ostringstream os;

    os << "{" << std::endl;
    reportNode(os, *root_, 1);
    os << "}";
    return os.str();
}

Profiler* Profiler::getDefault()
{
    static Profiler profiler;
    return &profiler;
}

NS_FXP_END
