//////////////////////////////////////////////////////////////////////
/// Desc  FBVHTree
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FBVHTree.hpp"
#include "FCollider.hpp"
#include "debug/DebugDraw.hpp"
#include "debug/Profiler.hpp"
#include <cassert>
#include <algorithm>

NS_FXP_BEGIN

static inline FFloat getCost(const FBB &a, const FBB &b, const FBB &c)
{
    FBB ac = a;
    ac.add(c);
    return b.getArea() + ac.getArea();
}

static inline FFloat getCostEx(const FBB &a, const FBB &b)
{
    return FMath::abs(a.min.x + a.max.x - b.min.x - b.max.x) + FMath::abs(a.min.y + a.max.y - b.min.y - b.max.y);
}

static inline void mergeBB(FBB &output, const FBB &a, const FBB &b)
{
    output = a;
    output.add(b);
}


// 自底向上更新包围盒
static void updateBBBottomUp(FBVHNode *node)
{
    while (node)
    {
        mergeBB(node->bb, node->left->bb, node->right->bb);
        node = node->parent;
    }
}

static inline FBVHNode* getNeighborNode(FBVHNode *node)
{
    FBVHNode* parent = node->parent;
    if (parent == nullptr)
    {
        return nullptr;
    }

    if (node == parent->left)
    {
        return parent->right;
    }
    else
    {
        return parent->left;
    }
}

FBVHTree::FBVHTree()
{
    stack.reserve(100);
}

FBVHTree::~FBVHTree()
{
    clear();

    while (freeList)
    {
        FBVHNode *p = freeList;
        freeList = freeList->left;
        delete p;
    }
}

void FBVHTree::addCollider(FCollider* collider)
{
    LS_PROFILER(PK_PHYSICS_BVH_ADD);
    auto it = colliderMap.find(collider);
    if (it != colliderMap.end())
    {
        LOG_ERROR("Collider %d already added to tree", collider->getID());
        return;
    }

    ++changedCount_;

    if (root == nullptr)
    {
        root = createLeaf(collider);
        return;
    }

    FBVHNode *node = root;
    while (!node->isLeafNode())
    {
        FFloat leftCost = getCost(node->left->bb, node->right->bb, collider->getBounds());
        FFloat rightCost = getCost(node->right->bb, node->left->bb, collider->getBounds());
        if (leftCost == rightCost)
        {
            leftCost = getCostEx(node->left->bb, collider->getBounds());
            rightCost = getCostEx(node->right->bb, collider->getBounds());
        }

        if (leftCost < rightCost)
        {
            node = node->left;
        }
        else
        {
            node = node->right;
        }
    }

    FBVHNode *parent = node->parent;

    FBVHNode *old = node;
    FBVHNode *leaf = createLeaf(collider);

    // 如果node是叶结点，则新建一个结点，替换掉node的位置
    node = createNode();
    node->setAsNode(old, leaf);

    if (parent == nullptr)
    {
        root = node;
    }
    else
    {
        node->parent = parent;
        if (old == parent->left)
        {
            parent->left = node;
        }
        else
        {
            parent->right = node;
        }
    }

    updateBBBottomUp(parent);
}

bool FBVHTree::removeCollider(FCollider * collider)
{
    LS_PROFILER(PK_PHYSICS_BVH_REMOVE);
    if (root == nullptr)
    {
        return false;
    }

    auto it = colliderMap.find(collider);
    if (it == colliderMap.end())
    {
        return false;
    }

    ++changedCount_;

    FBVHNode *node = it->second;
    assert(node->isLeafNode());

    colliderMap.erase(it);

    if (node == root)
    {
        releaseNode(root);
        root = nullptr;
        return true;
    }

    // 为了保持满二叉树，要将结点和其父结点一同删除
    // 将邻居结点提升到父级结点的位置，挂接到祖父结点下

    FBVHNode *neighbor = getNeighborNode(node);
    
    if (node->parent == root)
    {
        root = neighbor;
        neighbor->parent = nullptr;
    }
    else
    {
        FBVHNode *grandParent = node->parent->parent;
        if (node->parent == grandParent->left)
        {
            grandParent->left = neighbor;
        }
        else
        {
            assert(grandParent->right == node->parent);
            grandParent->right = neighbor;
        }
        neighbor->parent = grandParent;

        updateBBBottomUp(grandParent);
    }

    // 回收结点
    releaseNode(node->parent);
    releaseNode(node);
    return true;
}

void FBVHTree::updateCollider(FCollider *collider)
{
    LS_PROFILER(PK_PHYSICS_BVH_CHANGE);

    auto it = colliderMap.find(collider);
    if (it == colliderMap.end())
    {
        return;
    }

    FBVHNode *node = it->second;
    if (node->bb.contians(collider->getBounds()))
    {
        return;
    }

    // 删除后重新添加
    collider->retain();

    removeCollider(collider);
    addCollider(collider);
    
    collider->release();
}

void FBVHTree::clear()
{
    if (nullptr != root)
    {
        clearNode(root);
        root = nullptr;
    }

    colliderMap.clear();
    changedCount_ = 0;
}

void FBVHTree::clearNode(FBVHNode *node)
{
    if (node->left != nullptr)
    {
        clearNode(node->left);
    }
    if (node->right != nullptr)
    {
        clearNode(node->right);
    }

    releaseNode(node);
}

static int getNodeDepth(FBVHNode *node)
{
    if (node->isLeafNode())
    {
        return 1;
    }

    return 1 + FMath::max(
        getNodeDepth(node->left),
        getNodeDepth(node->right)
    );
}

size_t FBVHTree::getDepth()
{
    return root != nullptr ? getNodeDepth(root) : 0;
}

static int _getNodeCount(FBVHNode *node)
{
    if (node->isLeafNode())
    {
        return 1;
    }

    return 1 + _getNodeCount(node->left) + _getNodeCount(node->right);
}

size_t FBVHTree::getNodeCount()
{
    return root != nullptr ? _getNodeCount(root) : 0;
}

static void debugDrawNode(FBVHNode *node, int depth, int maxDepth)
{
    auto drawer = DebugDraw::getInstance();
    if (drawer->showBVHDepth < 0 || drawer->showBVHDepth == depth)
    {
        if ((drawer->showBVHLeaf && node->isLeafNode()) ||
            (drawer->showBVHNode && !node->isLeafNode()))
        {
            float r = float(depth) / maxDepth;
            drawer->drawBB(node->bb, Color(r, r, r));
        }
    }

    if (!node->isLeafNode())
    {
        debugDrawNode(node->left, depth + 1, maxDepth);
        debugDrawNode(node->right, depth + 1, maxDepth);
    }
}

void FBVHTree::debugDraw()
{
    if (nullptr == root)
    {
        return;
    }

    int maxDepth = (int)getDepth();
    debugDrawNode(root, 1, maxDepth);
}

size_t FBVHTree::getMemorySize()
{
    size_t ret = sizeof(*this) +
        colliderMap.size() * (sizeof(std::unordered_map<FCollider*, FBVHNode*>::value_type) + sizeof(FBVHNode)) +
        stack.capacity() * sizeof(FBVHQueryNode);
    
    FBVHNode *p = freeList;
    while (p)
    {
        ret += sizeof(FBVHNode);
        p = p->left;
    }
    
    return ret;
}

FBVHNode* FBVHTree::createNode()
{
    FBVHNode *ret;
    if (freeList != nullptr)
    {
        ret = freeList;
        freeList = freeList->left;
    }
    else
    {
        ret = new FBVHNode();
    }
    return ret;
}

void FBVHTree::releaseNode(FBVHNode *node)
{
    node->left = freeList;
    freeList = node;

    node->collider = nullptr;
    node->right = nullptr;
    node->parent = nullptr;
}

FBVHNode* FBVHTree::createLeaf(FCollider * collider)
{
    FBB bb = collider->getBounds();
    FVector2 expand = bb.getDiameter() * edgeCoef;
    bb.expand(expand.x, expand.y);

    FBVHNode *leaf = createNode();
    leaf->setAsLeaf(collider, bb);
    leaf->parent = nullptr;
    colliderMap[collider] = leaf;
    return leaf;
}

struct OPSortNodes
{
    int axis_;
    
    OPSortNodes(int axis)
        : axis_(axis)
    {}

    bool operator()(FBVHNode *a, FBVHNode *b)
    {
        return a->bb.getCenter()[axis_] < b->bb.getCenter()[axis_];
    }
};

void FBVHTree::rebuild()
{
    changedCount_ = 0;
    if (getNodeCount() < 7)
    {
        return;
    }

    std::vector<FBVHNode*> nodes;
    for (auto &pair : colliderMap)
    {
        nodes.push_back(pair.second);
    }

    releaseNoneLeafNodes(root);
    root = nullptr;

    root = rebuild(nodes.data(), nodes.data() + nodes.size(), 0);
}

void FBVHTree::releaseNoneLeafNodes(FBVHNode *node)
{
    if (node->isLeafNode())
    {
        return;
    }

    releaseNoneLeafNodes(node->left);
    releaseNoneLeafNodes(node->right);
    releaseNode(node);
}

FBVHNode* FBVHTree::rebuild(FBVHNode **start, FBVHNode **end, int axis)
{
    size_t n = end - start;
    if (n == 1)
    {
        return start[0];
    }
    if (n == 2)
    {
        FBVHNode *node = createNode();
        node->setAsNode(start[0], start[1]);
        return node;
    }

    // 按照AABB的中心点坐标进行排序
    OPSortNodes op(axis);
    std::stable_sort(start, end, op);
    axis = (axis + 1) % 2;

    // 半闭半开区间 [0, half)
    size_t half = n / 2 + 1;

    FBVHNode *left = rebuild(start, start + half, axis);
    FBVHNode *right = rebuild(start + half, end, axis);

    FBVHNode *node = createNode();
    node->setAsNode(left, right);
    return node;
}

NS_FXP_END
