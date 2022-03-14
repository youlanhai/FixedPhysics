//////////////////////////////////////////////////////////////////////
/// Desc  FBVHTree
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "FBB.hpp"
#include "FCollider.hpp"
#include "FRay.hpp"
#include "common/SmartPtr.hpp"
#include "debug/LogTool.hpp"
#include "debug/Profiler.hpp"

#include <unordered_map>
#include <bitset>
#include <string>
#include <vector>

NS_FXP_BEGIN

class FXP_API FBVHNode
{
public:
    /** 结点的包围盒 */
    FBB bb;
    /** 父结点 */
    FBVHNode* parent = nullptr;
    /** 左结点。回收到回收池的时候，当做链表的next字段使用。*/
    FBVHNode* left = nullptr;
    /** 右结点。*/
    FBVHNode* right = nullptr;
    /** 碰撞体。如果碰撞体不为空，则当前结点是叶结点；否则，不是叶结点，必定有两个子结点*/
    SmartPtr<FCollider> collider;

    void setAsLeaf(FCollider *collider, const FBB &bb)
    {
        this->collider = collider;
        left = nullptr;
        right = nullptr;
        this->bb = bb;
    }

    void setAsNode(FBVHNode *left, FBVHNode *right)
    {
        this->collider = nullptr;
        this->left = left;
        this->right = right;

        bb = left->bb;
        bb.add(right->bb);

        left->parent = this;
        right->parent = this;
    }

    inline bool isLeafNode()
    {
        return collider != nullptr;
    }
};

struct FBVHQueryNode
{
    FBVHNode *node;
    FBB bb;
    FVector2 start, end;
};

/** 层次包围盒树。是一颗满二叉树 */
class FXP_API FBVHTree
{
    DISABLE_COPY_AND_ASSIGN(FBVHTree);
public:
    FBVHTree();
    ~FBVHTree();

    FBVHNode* getRoot() { return root; }

    void addCollider(FCollider* collider);
    bool removeCollider(FCollider* collider);
    void updateCollider(FCollider *collider);

    /** 清空整个树 */
    void clear();

    /** 获得树的深度*/
    size_t getDepth();

    /** 获得树的结点总个数。包括叶结点 */
    size_t getNodeCount();
    size_t getLeafeCount() { return colliderMap.size(); }
    int getChangedCount() { return changedCount_; }

    /** 根据包围盒范围查询碰撞体。如果visit函数返回true，则终止查询；否则继续查找下一个匹配的碰撞体。*/
    template<typename T>
    bool queryCollider(const FBB & bb, T &visit);
    
    template<typename T>
    void queryByRay(const FVector2 &start, const FVector2 &direction, FFloat distance, T &visit);

    void debugDraw();
    
    size_t getMemorySize();

    /** 构造较慢，查询很快。适合静态物体 */
    void rebuild();

    void setEdgeCoef(FFloat coef) { edgeCoef = coef; }
    FFloat getEdgeCoef() const { return edgeCoef; }

private:
    void clearNode(FBVHNode *node);

    FBVHNode* createNode();

    void releaseNode(FBVHNode *node);

    FBVHNode* createLeaf(FCollider *collider);

    FBVHNode* rebuild(FBVHNode **start, FBVHNode **end, int axis);
    void releaseNoneLeafNodes(FBVHNode *node);
    
    template<typename T>
    void queryByRay2(FBVHNode *node, FFloat &minDistance, const FVector2 &start, const FVector2 &end, T &visit, int depth);

private:

    FBVHNode* root = nullptr;

    // 内存复用链表
    FBVHNode* freeList = nullptr;

    // 通过collider快速找到其挂接在的结点
    std::unordered_map<FCollider*, FBVHNode*> colliderMap;

    int changedCount_ = 0;

    // 包围盒的边界尺寸。将包围盒向外扩展一点，避免位置频繁变动引起树的重建。
    FFloat  edgeCoef = FFloat(0, 1);

    // 缓存
    std::vector<FBVHQueryNode> stack;
};


template<typename T>
bool FBVHTree::queryCollider(const FBB & bounds, T &visit)
{
    if (nullptr == root)
    {
        return false;
    }

    stack.clear();
    stack.push_back(FBVHQueryNode{ root, bounds });

    while (!stack.empty())
    {
        FBVHQueryNode top = stack.back();
        stack.pop_back();

        FBVHNode *node = top.node;
        if (!node->bb.intersect(top.bb))
        {
            continue;
        }

        if (node->isLeafNode())
        {
            // node->bb是向外扩展了的。需要与collider的bb再精确判断一次
            if (node->collider->getBounds().intersect(top.bb) && visit(node))
            {
                return true;
            }
        }
        else
        {
            FBB tempBB = top.bb;
            tempBB.sub(node->bb);

            stack.push_back(FBVHQueryNode{ node->left, tempBB });
            stack.push_back(FBVHQueryNode{ node->right, tempBB });
        }
    }
    return false;
}

#if 0
template<typename T>
void FBVHTree::queryByRay(const FVector2 &start, const FVector2 &end, T &visit, int depth)
{
    FBB unused;

    stack.clear();
    stack.push_back(FBVHQueryNode{ root, unused, start, end });

    while (!stack.empty())
    {
        FBVHQueryNode top = stack.back();
        stack.pop_back();

        FBVHNode *node = top.node;
        if (node->isLeafNode())
        {
            if (visit(node))
            {
                return;
            }
        }
        else
        {
            FVector2 s2 = top.start;
            FVector2 e2 = top.end;
            if (node->left->bb.clipLine(s2, e2))
            {
                stack.push_back(FBVHQueryNode{ node->left, unused, s2, e2 });
            }

            s2 = start;
            e2 = end;
            if (node->right->bb.clipLine(s2, e2))
            {
                stack.push_back(FBVHQueryNode{ node->right, unused, s2, e2 });
            }
        }
    }
}

#else
template<typename T>
void FBVHTree::queryByRay(const FVector2 &start, const FVector2 &direction, FFloat distance, T &visit)
{
    if (nullptr == root)
    {
        return;
    }

    FVector2 end = start + direction * distance;
    queryByRay2(root, distance, start, end, visit, 1);
}
#endif

template<typename T>
void FBVHTree::queryByRay2(FBVHNode *node, FFloat &minDistance, const FVector2 &start, const FVector2 &end, T &visit, int depth)
{
#if 0
    std::bitset<8> set(depth);
    std::string str = set.to_string();
    LOG_INFO("queryByRay2: %s, %d", str.c_str(), toi(minDistance));
#endif

    if (node->isLeafNode())
    {
        FFloat ret = visit(node);
        minDistance = FMath::min(ret, minDistance);
        return;
    }

    FFloat d1 = node->left->bb.getDistance(start, end);
    FFloat d2 = node->right->bb.getDistance(start, end);

    if (d1 < d2)
    {
        if (d1 < minDistance)
        {
            queryByRay2(node->left, minDistance, start, end, visit, depth << 1);
        }
        if (d2 < minDistance)
        {
            queryByRay2(node->right, minDistance, start, end, visit, depth << 1 | 1);
        }
    }
    else
    {
        if (d2 < minDistance)
        {
            queryByRay2(node->right, minDistance, start, end, visit, depth << 1 | 1);
        }
        if (d1 < minDistance)
        {
            queryByRay2(node->left, minDistance, start, end, visit, depth << 1);
        }
    }
}

NS_FXP_END
