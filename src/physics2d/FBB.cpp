//////////////////////////////////////////////////////////////////////
/// Desc  FBB
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FBB.hpp"
#include "math/FMath.hpp"
#include "math/FMatrix2D.hpp"
#include "FRay.hpp"
#include <algorithm>

NS_FXP_BEGIN

FBB::FBB(const FRay &ray)
{
    resetWithPoint(ray.start, ray.end);
}

void FBB::reset()
{
    min.set(FMath::FloatMax, FMath::FloatMax);
    max.set(FMath::FloatMin, FMath::FloatMin);
}

void FBB::resetWithPoint(const FVector2 &p0, const FVector2 &p1)
{
    if (p0.x < p1.x)
    {
        min.x = p0.x;
        max.x = p1.x;
    }
    else
    {
        min.x = p1.x;
        max.x = p0.x;
    }
    
    if (p0.y < p1.y)
    {
        min.y = p0.y;
        max.y = p1.y;
    }
    else
    {
        min.y = p1.y;
        max.y = p0.y;
    }
}

bool FBB::clipLine2(FVector2 & start, FVector2 & end) const
{
    FFloat tMin(0);
    FFloat tMax(1);

    FVector2 delta = end - start;

    for (int i = 0; i < 2; ++i)
    {
        if (delta[i] != 0)
        {
            FFloat dMin = (min[i] - start[i]) / delta[i];
            FFloat dMax = (max[i] - start[i]) / delta[i];

            if (dMin > dMax)
            {
                std::swap(dMin, dMax);
            }
            
            tMin = std::max(tMin, dMin);
            tMax = std::min(tMax, dMax);
        }
        else if (start[i] < min[i] || start[i] > max[i]) //起点不在包围盒范围。
            return false;
    }

    if (tMin > tMax || tMax < FFloat(0) || tMin > FFloat(1))
    {
        return false;
    }

    end = start + delta * tMax;
    start = start + delta * tMin;
    return true;
}

FFloat FBB::getDistance(const FVector2 & start, const FVector2 & end) const
{
    FFloat tMin(0);
    FFloat tMax(1);

    FVector2 delta = end - start;

    for (int i = 0; i < 2; ++i)
    {
        if (delta[i] != 0)
        {
            FFloat dMin = (min[i] - start[i]) / delta[i];
            FFloat dMax = (max[i] - start[i]) / delta[i];

            if (dMin > dMax)
            {
                std::swap(dMin, dMax);
            }

            tMin = std::max(tMin, dMin);
            tMax = std::min(tMax, dMax);
        }
        else if (start[i] < min[i] || start[i] > max[i]) //起点不在包围盒范围。
            return FMath::FloatMax;
    }

    if (tMin > tMax || tMax < FFloat(0) || tMin > FFloat(1))
    {
        return FMath::FloatMax;
    }

    return tMin;
}

/** 精确求交 */
bool FBB::clipLine(FVector2 & start, FVector2 & end) const
{
    const FVector2 o = start;
    FVector2 delta = end - start;

    if (delta.x > 0) // 射线方向从min指向max
    {
        if (start.x < min.x)
        {
            start.x = min.x;
            start.y = (start.x - o.x) * delta.y / delta.x + o.y;
        }
        if (end.x > max.x)
        {
            end.x = max.x;
            end.y = (end.x - o.x) * delta.y / delta.x + o.y;
        }
        if (start.x > end.x)
        {
            return false;
        }
    }
    else if (delta.x < 0)
    {
        if (start.x > max.x)
        {
            start.x = max.x;
            start.y = (start.x - o.x) * delta.y / delta.x + o.y;
        }
        if (end.x < min.x)
        {
            end.x = min.x;
            end.y = (end.x - o.x) * delta.y / delta.x + o.y;
        }
        if (start.x < end.x)
        {
            return false;
        }
    }
    else
    {
        if (start.x < min.x || start.x > max.x)
        {
            return false;
        }
    }

    if (delta.y > 0)
    {
        if (start.y < min.y)
        {
            start.y = min.y;
            start.x = (start.y - o.y) * delta.x / delta.y + o.x;
        }
        if (end.y > max.y)
        {
            end.y = max.y;
            end.x = (end.y - o.y) * delta.x / delta.y + o.x;
        }
        if (start.y > end.y)
        {
            return false;
        }
    }
    else if (delta.y < 0)
    {
        if (start.y > max.y)
        {
            start.y = max.y;
            start.x = (start.y - o.y) * delta.x / delta.y + o.x;
        }
        if (end.y < min.y)
        {
            end.y = min.y;
            end.x = (end.y - o.y) * delta.x / delta.y + o.x;
        }
        if (start.y < end.y)
        {
            return false;
        }
    }
    else
    {
        if (start.y < min.y || start.y > max.y)
        {
            return false;
        }
    }

    return true;
}

void FBB::normalize()
{
    if (min.x > max.x)
    {
        std::swap(min.x, max.x);
    }
    if (min.y > max.y)
    {
        std::swap(min.y, max.y);
    }

    if (max.x == min.x)
    {
        min.x -= FMath::DISTANCE_EPSILON;
        max.x += FMath::DISTANCE_EPSILON;
    }
    if (max.y == min.y)
    {
        min.y -= FMath::DISTANCE_EPSILON;
        max.y += FMath::DISTANCE_EPSILON;
    }
}

void FBB::applyMatrix(const FMatrix2D & mat)
{
    FVector2 corners[4];
    // Left-top.
    corners[0].set(min.x, max.y);
    // Left-bottom.
    corners[1].set(min.x, min.y);
    // Right-bottom.
    corners[2].set(max.x, min.y);
    // Right-top.
    corners[3].set(max.x, max.y);

    reset();
    for(int i = 0; i < 4; ++i)
    {
        corners[i] = mat.transformPoint(corners[i]);
        add(corners[i]);
    }
}

NS_FXP_END
