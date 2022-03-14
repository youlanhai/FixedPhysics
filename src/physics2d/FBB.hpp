//////////////////////////////////////////////////////////////////////
/// Desc  FBB
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "math/FVector2.hpp"

NS_FXP_BEGIN

class FMatrix2D;
class FRay;

class FXP_API FBB
{
public:
    FVector2 min;
    FVector2 max;

    FBB()
    {}
    
    FBB(const FVector2 &v)
        : min(v)
        , max(v)
    {
    }
    
    FBB(const FRay &ray);

    FBB(const FVector2 & min_, const FVector2 & max_)
        : min(min_) , max(max_)
    {}

    FBB(const FVector2 & center, FFloat radius)
    {
        min.x = center.x - radius;
        min.y = center.y - radius;

        max.x = center.x + radius;
        max.y = center.y + radius;
    }
    
    bool isValid() const { return min.x <= max.x &&  min.y <= max.y;  }

    void reset();
    
    void resetWithPoint(const FVector2 &p) { min = max = p; }
    
    void resetWithPoint(const FVector2 &p0, const FVector2 &p1);

    //正规化。避免min大于max，避免某个维度为0了。
    void normalize();

    FVector2 getCenter() const;
    void getCenter(FVector2 & center) const;

    FVector2 getDiameter() const;
    void getDiameter(FVector2 & diameter) const;

    ///包围盒求交集
    void sub(const FBB & aabb);

    ///包围盒求并集
    void add(const FBB & aabb);

    ///并入点
    void add(const FVector2 & pt);
    
    /** 边界向外扩张 */
    void expand(FFloat dx, FFloat dy);
    
    bool intersect(const FBB & aabb) const;
    bool contians(const FBB &aabb) const;

    /** 精确的裁减 */
    bool clipLine(FVector2 & start, FVector2 & end) const;
    bool clipLine2(FVector2 & start, FVector2 & end) const;

    /** 获取射线到包围盒的距离 */
    FFloat getDistance(const FVector2 & start, const FVector2 & end) const;

    void applyMatrix(const FMatrix2D & mat);
    void applyMatrix(FBB & out, const FMatrix2D & mat) const;

    FFloat getArea() const { return (max.x - min.x) * (max.y - min.y); }

    bool operator == (const FBB &bb) const { return min == bb.min && max == bb.max; }
    bool operator != (const FBB &bb) const { return min != bb.min || max != bb.max; }
};

inline bool FBB::intersect(const FBB & aabb) const
{
    return !(max.x < aabb.min.x ||
        max.y < aabb.min.y ||
        min.x > aabb.max.x ||
        min.y > aabb.max.y);
}

inline bool FBB::contians(const FBB & aabb) const
{
    return !(aabb.min.x < min.x ||
        aabb.max.x > max.x ||
        aabb.min.y < min.y ||
        aabb.max.y > max.y);
}

inline void FBB::sub(const FBB & aabb)
{
    min.x = FMath::max(min.x, aabb.min.x);
    min.y = FMath::max(min.y, aabb.min.y);

    max.x = FMath::min(max.x, aabb.max.x);
    max.y = FMath::min(max.y, aabb.max.y);
}

inline void FBB::applyMatrix(FBB & output, const FMatrix2D & matrix) const
{
    output = *this;
    output.applyMatrix(matrix);
}

inline void FBB::add(const FBB & aabb)
{
    min.x = FMath::min(min.x, aabb.min.x);
    min.y = FMath::min(min.y, aabb.min.y);

    max.x = FMath::max(max.x, aabb.max.x);
    max.y = FMath::max(max.y, aabb.max.y);
}

inline void FBB::add(const FVector2 & pt)
{
    min.x = FMath::min(min.x, pt.x);
    min.y = FMath::min(min.y, pt.y);

    max.x = FMath::max(max.x, pt.x);
    max.y = FMath::max(max.y, pt.y);
}


inline void FBB::expand(FFloat dx, FFloat dy)
{
    min.x -= dx;
    max.x += dx;
    min.y -= dy;
    max.y += dy;
}

inline FVector2 FBB::getCenter() const
{
    return (max + min) / FFloat(2);
}

inline void FBB::getCenter(FVector2 & center) const
{
    center = (max + min) / FFloat(2);
}

inline FVector2 FBB::getDiameter() const
{
    return max - min;
}

inline void FBB::getDiameter(FVector2 & diameter) const
{
    diameter = max - min;
}

NS_FXP_END
