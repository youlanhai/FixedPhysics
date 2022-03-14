//////////////////////////////////////////////////////////////////////
/// Desc  FVector3
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once

#include "FVector2.hpp"

NS_FXP_BEGIN

class FXP_API FVector3
{
public:
    static const FVector3 Zero;
    static const FVector3 One;
    static const FVector3 XAxis;
    static const FVector3 YAxis;
    static const FVector3 ZAxis;
    
    FFloat x, y, z;

    FVector3()
    {}

    FVector3(Fixed32 _x, Fixed32 _y, Fixed32 _z)
        : x(_x), y(_y), z(_z)
    {}

    inline bool isZero() const
    {
        return x == 0 && y == 0 && z == 0;
    }

    inline void set(FFloat _x, FFloat _y, FFloat _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    inline FFloat operator[](size_t i) const {return ((FFloat*)this)[i]; }
    inline FFloat& operator[](size_t i) {return ((FFloat*)this)[i]; }

    inline FVector3 operator - () const {return FVector3(-x, -y, -z); }
    
    inline FVector3 operator * (FFloat v) const {return FVector3(x * v, y * v, z * v); }
    inline FVector3 operator / (FFloat v) const {return FVector3(x / v, y / v, z / v); }

    inline const FVector3& operator *= (FFloat v) {x *= v; y *= v; z *= v; return *this; }
    inline const FVector3& operator /= (FFloat v) {x /= v; y /= v; z /= v; return *this; }
    
    inline FVector3 operator + (const FVector3 &v) const { return FVector3(x + v.x, y + v.y, z + v.z); }
    inline FVector3 operator - (const FVector3 &v) const { return FVector3(x - v.x, y - v.y, z - v.z); }
    inline FVector3 operator * (const FVector3 &v) const { return FVector3(x * v.x, y * v.y, z * v.z); }
    inline FVector3 operator / (const FVector3 &v) const { return FVector3(x / v.x, y / v.y, z / v.z); }

    inline const FVector3& operator += (const FVector3 &v) { x += v.x; y += v.y; z += v.z; return *this; }
    inline const FVector3& operator -= (const FVector3 &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    inline const FVector3& operator *= (const FVector3 &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
    inline const FVector3& operator /= (const FVector3 &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

    inline bool operator == (const FVector3 &v) const { return x == v.x && y == v.y && z == v.z; }
    inline bool operator != (const FVector3 &v) const { return x != v.x || y != v.y || z != v.z; }

    inline FFloat dot(const FVector3 &v) const;
    inline FFloat crossXZ(const FVector3 &v) const;

    inline FFloat lengthSq() const;
    inline FFloat length() const;

    inline FFloat distanceToSq(const FVector3 &v) const;
    inline FFloat distanceTo(const FVector3 &v) const;

    inline void normalizeTo(FVector3 &output) const;
    inline void normalize();
    
    bool almostEqual(const FVector3 &other, FFloat epsilon = FMath::DEFAULT_EPSILON) const
    {
        FVector3 v = *this - other;
        return (v.x >= -epsilon && v.x <= epsilon) &&
            (v.y >= -epsilon && v.y <= epsilon) &&
            (v.z >= -epsilon && v.z <= epsilon);
    }

    bool almostZero(FFloat epsilon = FMath::DEFAULT_EPSILON) const;
    
    inline FVector2 toXZ() const
    {
        return FVector2(x, z);
    }
    
    inline void setXZ(const FVector2 &v)
    {
        x = v.x;
        z = v.y;
    }
    
    inline static FVector3 FromXZ(const FVector2 &v)
    {
        return FVector3(v.x, FFloat(0), v.y);
    }
};

inline FFloat FVector3::dot(const FVector3 &v) const
{
    //return x * v.x + y * v.y * z * v.z;
    int64_t t = int64_t(x.value) * v.x.value + int64_t(y.value) * v.y.value + int64_t(z.value) * v.z.value;
    return Fixed32(true, (int)Fixed32::down(t));
}

inline FFloat FVector3::crossXZ(const FVector3 &v) const
{
    //return x * v.z - z * v.x;
    int64_t t = int64_t(x.value) * v.z.value - int64_t(z.value) * v.x.value;
    return Fixed32(true, (int)Fixed32::down(t));
}

inline FFloat FVector3::lengthSq() const
{
    //return dot(*this);
    int64_t t = int64_t(x.value) * x.value + int64_t(y.value) * y.value + int64_t(z.value) * z.value;
    return Fixed32(true, (int)Fixed32::down(t));
}

inline FFloat FVector3::length() const
{
    // return FMath::sqrt(lengthSq());
    int64_t t = int64_t(x.value) * x.value + int64_t(y.value) * y.value + int64_t(z.value) * z.value;
    return Fixed32(true, FMath::sqrt(t));
}

inline FFloat FVector3::distanceToSq(const FVector3 &v) const
{
    //FVector3 t = *this - v;
    //return t.lengthSq();
    FFloat dx = x - v.x, dy = y - v.y, dz = z - v.z;
    int64_t t = int64_t(dx.value) * dx.value + int64_t(dy.value) * dy.value + int64_t(dz.value) * dz.value;
    return Fixed32(true, (int)Fixed32::down(t));
}

inline FFloat FVector3::distanceTo(const FVector3 &v) const
{
    //return FMath::sqrt(distanceToSq(v));
    FFloat dx = x - v.x, dy = y - v.y, dz = z - v.z;
    int64_t t = int64_t(dx.value) * dx.value + int64_t(dy.value) * dy.value + int64_t(dz.value) * dz.value;
    return Fixed32(true, FMath::sqrt(t));
}

inline void FVector3::normalizeTo(FVector3 &output) const
{
    FFloat l = length();
    if (l > 0)
    {
        output.x = x / l;
        output.y = y / l;
        output.z = z / l;
    }
    else
    {
        output.x = 0;
        output.y = 0;
        output.z = 0;
    }
}

inline void FVector3::normalize()
{
    // normalizeTo(*this);
    FFloat l = length();
    if (l > 0)
    {
        x = x / l;
        y = y / l;
        z = z / l;
    }
    else
    {
        x = 0;
        y = 0;
        z = 0;
    }
}

inline bool FVector3::almostZero(FFloat epsilon) const
{
    return !(x > epsilon || x < -epsilon ||
        y > epsilon || y < -epsilon ||
        z > epsilon || z < -epsilon);
}

NS_FXP_END
