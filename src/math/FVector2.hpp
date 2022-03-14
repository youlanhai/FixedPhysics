//////////////////////////////////////////////////////////////////////
/// Desc  FVector2
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FMath.hpp"

NS_FXP_BEGIN

class FXP_API FVector2
{
public:
    static const FVector2 ZERO;

    FFloat x, y;

    FVector2()
    {}

    FVector2(FFloat _x, FFloat _y)
        : x(_x), y(_y)
    {}

    bool isZero() const
    {
        return x == 0 && y == 0;
    }

    void set(FFloat _x, FFloat _y)
    {
        x = _x;
        y = _y;
    }

    FFloat operator[](size_t i) const {return ((FFloat*)this)[i]; }
    FFloat& operator[](size_t i) {return ((FFloat*)this)[i]; }

    FVector2 operator - () const {return FVector2(-x, -y); }

    FVector2 operator * (FFloat v) const {return FVector2(x * v, y * v); }
    FVector2 operator / (FFloat v) const {return FVector2(x / v, y / v); }

    const FVector2& operator *= (FFloat v) {x *= v; y *= v; return *this; }
    const FVector2& operator /= (FFloat v) {x /= v; y /= v; return *this; }
    
    FVector2 operator + (const FVector2 &v) const { return FVector2(x + v.x, y + v.y); }
    FVector2 operator - (const FVector2 &v) const { return FVector2(x - v.x, y - v.y); }
    FVector2 operator * (const FVector2 &v) const { return FVector2(x * v.x, y * v.y); }
    FVector2 operator / (const FVector2 &v) const { return FVector2(x / v.x, y / v.y); }

    const FVector2& operator += (const FVector2 &v) { x += v.x; y += v.y; return *this; }
    const FVector2& operator -= (const FVector2 &v) { x -= v.x; y -= v.y; return *this; }
    const FVector2& operator *= (const FVector2 &v) { x *= v.x; y *= v.y; return *this; }
    const FVector2& operator /= (const FVector2 &v) { x /= v.x; y /= v.y; return *this; }

    bool operator == (const FVector2 &v) const { return x == v.x && y == v.y; }
    bool operator != (const FVector2 &v) const { return x != v.x || y != v.y; }

    inline FFloat dot(const FVector2 &v) const;
    inline FFloat cross(const FVector2 &v) const;

    inline FFloat lengthSq() const;
    inline FFloat length() const;

    inline FFloat distanceToSq(const FVector2 &v) const;
    inline FFloat distanceTo(const FVector2 &v) const;

    inline void normalizeTo(FVector2 &output) const;

    /** 单位化。*/
    inline void normalize();

    /** 角度转换成方向 */
    static FVector2 fromAngle(FFloat angle)
    {
        return FVector2(FMath::cos(angle), FMath::sin(angle));
    }
};

inline FFloat FVector2::dot(const FVector2 &v) const
{
    //return x * v.x + y * v.y;
    int64_t t = int64_t(x.value) * v.x.value + int64_t(y.value) * v.y.value;
    return Fixed32(true, (int)Fixed32::down(t));
}

inline FFloat FVector2::cross(const FVector2 &v) const
{
    //return x * v.y - y * v.x;
    int64_t t = int64_t(x.value) * v.y.value - int64_t(y.value) * v.x.value;
    return Fixed32(true, (int)Fixed32::down(t));
}

inline FFloat FVector2::lengthSq() const
{
    //return dot(*this); 向外输出参数的方法，arm上会inline失败
    int64_t t = int64_t(x.value) * x.value + int64_t(y.value) * y.value;
    return Fixed32(true, (int)Fixed32::down(t));
}

inline FFloat FVector2::length() const
{
    //return FMath::sqrt(lengthSq());
    int64_t t = int64_t(x.value) * x.value + int64_t(y.value) * y.value;
    // a = Fixed32(true, (int)Fixed32::down(t));
    // Fixed32(true, sqrt(Fixed32::up64(a.value)));
    return Fixed32(true, FMath::sqrt(t));
}

inline FFloat FVector2::distanceToSq(const FVector2 &v) const
{
    //FVector2 t(x - v.x, y - v.y);
    //return t.lengthSq(); arm上会inline失败
    FFloat dx = x - v.x, dy = y - v.y;
    int64_t t = int64_t(dx.value) * dx.value + int64_t(dy.value) * dy.value;
    return Fixed32(true, (int)Fixed32::down(t));
}

inline FFloat FVector2::distanceTo(const FVector2 &v) const
{
    //return FMath::sqrt(distanceToSq(v));
    FFloat dx = x - v.x, dy = y - v.y;
    int64_t t = int64_t(dx.value) * dx.value + int64_t(dy.value) * dy.value;
    // a = Fixed32(true, (int)Fixed32::down(t));
    // Fixed32(true, sqrt(Fixed32::up64(a.value)));
    return Fixed32(true, FMath::sqrt(t));
}

inline void FVector2::normalizeTo(FVector2 &output) const
{
    FFloat l = length();
    if (l > 0)
    {
        output.x = x / l;
        output.y = y / l;
    }
    else
    {
        output.x = 0;
        output.y = 0;
    }
}

/** 单位化。*/
inline void FVector2::normalize()
{
    // normalizeTo(*this); 向外输出参数的方法，arm上会inline失败
    FFloat l = length();
    if (l > 0)
    {
        x = x / l;
        y = y / l;
    }
    else
    {
        x = 0;
        y = 0;
    }
}
NS_FXP_END
