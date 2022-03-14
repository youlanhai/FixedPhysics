//////////////////////////////////////////////////////////////////////
/// Desc  FGJK
/// Time  2020/12/11
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once

#include "math/FVector2.hpp"
#include "math/FMath.hpp"
#include <vector>

NS_FXP_BEGIN

class FCollider;
class Simplex;
class SimplexEdge;

struct SupportPoint
{
    FVector2 point;
    FVector2 fromA;
    FVector2 fromB;
};

class FGJK
{
private:
    Simplex *simplex = nullptr;
    SimplexEdge *simplexEdge = nullptr;

    FCollider* shapeA = nullptr;
    FCollider* shapeB = nullptr;
    /// 最大迭代次数
    int maxIterCount = 10;
    /// 浮点数误差。
    FFloat epsilon = FMath::DEFAULT_EPSILON;

    /// 当前support使用的方向
    FVector2 direction;

public:
    bool isCollision = false;
    // 最近点
    FVector2 closestOnA;
    FVector2 closestOnB;

    FVector2 penetrationNormal;
    FFloat penetrationDistance;

    FGJK();

    bool queryCollision(FCollider* shapeA, FCollider* shapeB);
    size_t getMemorySize();

private:
    SupportPoint support(const FVector2 &dir);

    FVector2 findFirstDirection();

    FVector2 findNextDirection();

    void computeClosetPoint(const SupportPoint &A, const SupportPoint &B);

    void queryEPA();
};

bool containsPoint(const FVector2 * points, size_t count, const FVector2 & point);

NS_FXP_END

