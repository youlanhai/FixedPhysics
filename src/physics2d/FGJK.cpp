//////////////////////////////////////////////////////////////////////
/// Desc  FGJK
/// Time  2020/12/11
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FGJK.hpp"
#include "FCollider.hpp"
#include "debug/Profiler.hpp"

NS_FXP_BEGIN

struct Edge
{
    SupportPoint a;
    SupportPoint b;
    FVector2 normal;
    FFloat distance;
    int index;
};

/// 判读点c在ab的哪一侧
inline int whitchSide(const FVector2 &a, const FVector2 &b, const FVector2 &c)
{
    FVector2 ab = b - a;
    FVector2 ac = c - a;
    FFloat cross = ab.cross(ac); // ab.x * ac.y - ab.y * ac.x;
    return cross > 0 ? 1 : (cross < 0 ? -1 : 0);
}

/// 获得原点到线段ab的最近点。最近点可以是垂点，也可以是线段的端点。
inline FVector2 getClosestPointToOrigin(const FVector2 &a, const FVector2 &b)
{
    FVector2 ab = b - a;
    FVector2 ao = - a; // origin - a

    FFloat sqrLength = ab.lengthSq();

    // ab点重合了
    if (sqrLength == 0)
    {
        return a;
    }

    FFloat projection = ab.dot(ao) / sqrLength;
    projection = FMath::clamp01(projection);
    return a + ab * projection;
}

/// 获得原点到直线ab的垂点
inline FVector2 getPerpendicularToOrigin(const FVector2 &a, const FVector2 &b)
{
    FVector2 ab = b - a;
    FVector2 ao = -a; // origin - a

    FFloat sqrLength = ab.lengthSq();

    // ab点重合了
    if (sqrLength == 0)
    {
        return a;
    }

    FFloat projection = ab.dot(ao) / sqrLength;
    return a + ab * projection;
}

bool containsPoint(const FVector2 * points, size_t count, const FVector2 & point)
{
    if (count < 3)
    {
        return false;
    }

    // 先计算出内部的方向
    int innerSide = whitchSide(points[0], points[1], points[2]);

    // 通过判断点是否均在三条边的内侧，来判定单形体是否包含点
    for (size_t i = 0; i < count; ++i)
    {
        size_t iNext = (i + 1) % count;
        int side = whitchSide(points[i], points[iNext], point);

        if (side == 0) // 在边界上
        {
            return true;
        }

        if (side != innerSide) // 在外部
        {
            return false;
        }
    }

    return true;
}

inline bool containsPoint(const std::vector<FVector2> &points, const FVector2 &point)
{
    return containsPoint(points.data(), points.size(), point);
}

inline SupportPoint supportPoint(FCollider *shapeA, FCollider *shapeB, const FVector2 &dir)
{
    FVector2 a = shapeA->getFarthestPointInDirection(dir);
    FVector2 b = shapeB->getFarthestPointInDirection(-dir);
    return SupportPoint
    {
        a - b,
        a,
        b,
    };
}



class Simplex
{
    std::vector<FVector2> points;
    std::vector<FVector2> fromA;
    std::vector<FVector2> fromB;
public:

    void clear()
    {
        points.clear();
        fromA.clear();
        fromB.clear();
    }

    int count() const
    {
        return (int)points.size();
    }

    const FVector2& get(int i) const
    {
        return points[i];
    }

    const FVector2& getLast() const
    {
        return points.back();
    }

    SupportPoint getSupport(int i) const
    {
        return SupportPoint
        {
            points[i],
            fromA[i],
            fromB[i],
        };
    }

    void add(const SupportPoint &point)
    {
        points.push_back(point.point);
        fromA.push_back(point.fromA);
        fromB.push_back(point.fromB);
    }

    void remove(int index)
    {
        points.erase(points.begin() + index);
        fromA.erase(fromA.begin() + index);
        fromB.erase(fromB.begin() + index);
    }

    bool contains(const FVector2 &point) const
    {
        return containsPoint(points, point);
    }
};

class SimplexEdge
{
public:
    std::vector<Edge> edges;

    SimplexEdge()
    {}

    ~SimplexEdge()
    {}

    inline void clear()
    {
        edges.clear();
    }

    inline void initEdges(const Simplex *simplex)
    {
        edges.clear();

        edges.push_back(createInitEdge(simplex->getSupport(0), simplex->getSupport(1)));
        edges.push_back(createInitEdge(simplex->getSupport(1), simplex->getSupport(0)));

        updateEdgeIndex();
    }

    inline const Edge* findClosestEdge() const
    {
        FFloat minDistance = FMath::FloatMax;
        const Edge *ret = nullptr;
        for (const Edge &e : edges)
        {
            if (e.distance < minDistance)
            {
                ret = &e;
                minDistance = e.distance;
            }
        }
        return ret;
    }

    inline void insertEdgePoint(const Edge *p, const SupportPoint & point)
    {
        Edge e = *p;

        Edge e1 = createEdge(e.a, point);
        edges[e.index] = e1;

        Edge e2 = createEdge(point, e.b);
        edges.insert(edges.begin() + e.index + 1, e2);

        updateEdgeIndex();
    }

    inline void updateEdgeIndex()
    {
        for (int i = 0; i < edges.size(); ++i)
        {
            edges[i].index = i;
        }
    }

    inline Edge createEdge(const SupportPoint & a, const SupportPoint & b)
    {
        Edge e;
        e.a = a;
        e.b = b;
        e.index = 0;
        e.normal = getPerpendicularToOrigin(a.point, b.point);
        e.distance = e.normal.length();

        // 单位化边
        if (e.distance > 0)
        {
            e.normal /= e.distance;
            e.normal.normalize(); // 再单位化一次，减少误差
        }
        else
        {
            // 如果距离原点太近，用数学的方法来得到直线的垂线
            // 方向可以随便取，刚好另外一边是反着来的
            FVector2 v = a.point - b.point;
            e.normal.set(-v.y, v.x);
            e.normal.normalize();
        }
        return e;
    }

    inline Edge createInitEdge(const SupportPoint & a, const SupportPoint & b)
    {
        Edge e;
        e.a = a;
        e.b = b;
        e.index = 0;

        FVector2 perp = getPerpendicularToOrigin(a.point, b.point);
        e.distance = perp.length();

        // 如果距离原点太近，用数学的方法来得到直线的垂线
        // 方向可以随便取，刚好另外一边是反着来的
        FVector2 v = a.point - b.point;
        e.normal.set(-v.y, v.x);
        e.normal.normalize();
        return e;
    }
};



FGJK::FGJK()
{
    simplex = new Simplex();
    simplexEdge = new SimplexEdge();
}

bool FGJK::queryCollision(FCollider* shapeA, FCollider* shapeB)
{
    this->shapeA = shapeA;
    this->shapeB = shapeB;

    simplex->clear();
    isCollision = false;
    direction = FVector2::ZERO;

    closestOnA = FVector2::ZERO;
    closestOnB = FVector2::ZERO;

    simplexEdge->clear();
    penetrationNormal = FVector2::ZERO;
    penetrationDistance = FFloat(0);

    LS_PROFILER_BEGIN(PK_PHYSICS_GJK_ONLY);

    direction = findFirstDirection();
    simplex->add(support(direction));
    simplex->add(support(-direction));

    direction = -getClosestPointToOrigin(simplex->get(0), simplex->get(1));
    for (int i = 0; i < maxIterCount; ++i)
    {
        // 方向接近于0，说明原点就在边上
        if (direction.lengthSq() < epsilon)
        {
            isCollision = true;
            break;
        }

        SupportPoint p = support(direction);
        // 新点与之前的点重合了。也就是沿着dir的方向，已经找不到更近的点了。
        if (p.point.distanceToSq(simplex->get(0)) < epsilon ||
            p.point.distanceToSq(simplex->get(1)) < epsilon)
        {
            isCollision = false;
            break;
        }

        simplex->add(p);

        // 单形体包含原点了
        if (simplex->contains(FVector2::ZERO))
        {
            isCollision = true;
            break;
        }

        direction = findNextDirection();
    }

    LS_PROFILER_END(PK_PHYSICS_GJK_ONLY);

    if (!isCollision)
    {
        computeClosetPoint(simplex->getSupport(0), simplex->getSupport(1));
    }
    else
    {
        LS_PROFILER_BEGIN(PK_PHYSICS_EPA_ONLY);
        queryEPA();
        LS_PROFILER_END(PK_PHYSICS_EPA_ONLY);
    }

    return isCollision;
}

void FGJK::queryEPA()
{
    if (simplex->count() > 2)
    {
        findNextDirection();
    }

    // EPA算法计算穿透向量
    simplexEdge->initEdges(simplex);

    const Edge* currentEdge = nullptr;

    for (int i = 0; i < maxIterCount; ++i)
    {
        const Edge *e = simplexEdge->findClosestEdge();

        currentEdge = e;
        penetrationNormal = e->normal;
        penetrationDistance = e->distance;

        SupportPoint sp = supportPoint(shapeA, shapeB, e->normal);
        FFloat distance = sp.point.dot(e->normal);
        if (distance - e->distance < epsilon)
        {
            break;
        }

        if (sp.point.distanceToSq(e->a.point) < epsilon ||
            sp.point.distanceToSq(e->b.point) < epsilon)
        {
            break;
        }

        simplexEdge->insertEdgePoint(e, sp);
    }

    computeClosetPoint(currentEdge->a, currentEdge->b);
}

SupportPoint FGJK::support(const FVector2 &dir)
{
    return supportPoint(shapeA, shapeB, dir);
}

FVector2 FGJK::findFirstDirection()
{
    FVector2 pointA = shapeA->getBounds().getCenter();
    FVector2 pointB = shapeB->getBounds().getCenter();

    FVector2 dir = pointA - pointB;
    if (dir.lengthSq() < epsilon) // 避免首次取到的点距离为0
    {
        dir = shapeA->getFirstVertex() - pointB;
    }
    return dir;
}

FVector2 FGJK::findNextDirection()
{
    if (simplex->count() == 2)
    {
        FVector2 crossPoint = getClosestPointToOrigin(simplex->get(0), simplex->get(1));
        // 取靠近原点方向的向量
        return FVector2::ZERO - crossPoint;
    }
    else if (simplex->count() == 3)
    {
        FVector2 crossOnCA = getClosestPointToOrigin(simplex->get(2), simplex->get(0));
        FVector2 crossOnCB = getClosestPointToOrigin(simplex->get(2), simplex->get(1));

        // 保留距离原点近的，移除较远的那个点
        if (crossOnCA.lengthSq() < crossOnCB.lengthSq())
        {
            simplex->remove(1);
            return FVector2::ZERO - crossOnCA;
        }
        else
        {
            simplex->remove(0);
            return FVector2::ZERO - crossOnCB;
        }
    }
    else
    {
        // 不应该执行到这里
        return FVector2::ZERO;
    }
}

void FGJK::computeClosetPoint(const SupportPoint &A, const SupportPoint &B)
{
    /*
     *  L = AB，是Minkowski差集上的一个边，同时构成A、B两点的顶点也来自各自shape的边。
     *  E1 = Aa - Ba，E2 = Ab - Bb
     *  则求两个凸包的最近距离，就演变成了求E1和E2两个边的最近距离。
     *
     *  设Q点是原点到L的垂点，则有:
     *      L = B - A
     *      Q · L = 0
     *  因为Q是L上的点，可以用r1, r2来表示Q (r1 + r2 = 1)，则有: Q = A * r1 + B * r2
     *      (A * r1 + B * r2) · L = 0
     *  用r2代替r1: r1 = 1 - r2
     *      (A - A * r2 + B * r2) · L = 0
     *      (A + (B - A) * r2) · L = 0
     *      L · A + L · L * r2 = 0
     *      r2 = -(L · A) / (L · L)
     */

    FVector2 L = B.point - A.point;
    FFloat sqrDistanceL = L.lengthSq();
    // support点重合了
    if (sqrDistanceL < epsilon)
    {
        closestOnA = closestOnB = A.point;
    }
    else
    {
        FFloat r2 = -L.dot(A.point) / sqrDistanceL;
        r2 = FMath::clamp01(r2);
        FFloat r1 = FFloat(1) - r2;

        closestOnA = A.fromA * r1 + B.fromA * r2;
        closestOnB = A.fromB * r1 + B.fromB * r2;
    }
}

size_t FGJK::getMemorySize()
{
    return sizeof(*this) + sizeof(Simplex) + sizeof(SimplexEdge);
}

NS_FXP_END
