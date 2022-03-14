//////////////////////////////////////////////////////////////////////
/// Desc  FMatrix
/// Time  2020/12/04
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "FMath.hpp"
#include "FVector2.hpp"
#include "FVector3.hpp"

NS_FXP_BEGIN

// 3 x 2 行矩阵
class FMatrix2D
{
public:
    static const FMatrix2D IDENTITY;

    FFloat a, b;
    FFloat c, d;
    FFloat x, y;

    FMatrix2D()
    {}

    FMatrix2D(FFloat _a, FFloat _b, FFloat _c, FFloat _d, FFloat _x, FFloat _y)
        : a(_a), b(_b), c(_c), d(_d), x(_x), y(_y)
    {}

    void setIdentity()
    {
        *this = IDENTITY;
    }

    /** 使用变换参数构造矩阵
     *  @param position 位置
     *  @param rotation 旋转角度。单位: 度
     *  @param scale 缩放
    */
    void setTransform(const FVector2 &position, FFloat rotation, const FVector2& scale)
    {
        FFloat sinR = FMath::sin(rotation);
        FFloat cosR = FMath::cos(rotation);

        a = cosR * scale.x; b = sinR * scale.x;
        c = -sinR * scale.y; d = cosR * scale.y;
        x = position.x; y = position.y;
    }

    void setTranslate(const FVector2 &position)
    {
        a = FFloat(0); b = FFloat(0);
        c = FFloat(0); d = FFloat(0);
        x = position.x; y = position.y;
    }

    // x` = x * cosR - y * sinR
    // y` = x * sinR + y * cosR
    void setRotate(FFloat radian)
    {
        FFloat sinR = FMath::sin(radian);
        FFloat cosR = FMath::cos(radian);

        a = cosR; b = sinR;
        c = -sinR; d = cosR;
        x = FFloat(0); y = FFloat(0);
    }

    void setScale(const FVector2 &scale)
    {
        a = scale.x; b = FFloat(0);
        c = FFloat(0); d = scale.y;
        x = FFloat(0); y = FFloat(0);
    }

    void inverseFrom(const FMatrix2D &t)
    {
        FFloat invDet = FFloat(1) / (t.a * t.d - t.b * t.c);
        a =  t.d * invDet;
        b = -t.b * invDet;
        c = -t.c * invDet;
        d =  t.a * invDet;
        x = (t.c * y - t.d * x) * invDet;
        y = (t.b * x - t.a * y) * invDet;
    }

    void inverse()
    {
        FMatrix2D t = *this;
        inverseFrom(t);
    }

    void multiply(const FMatrix2D &t1, const FMatrix2D &t2)
    {
        a = t1.a * t2.a + t1.b * t2.c; b = t1.a * t2.b + t1.b * t2.d;
        c = t1.c * t2.a + t1.d * t2.c; d = t1.c * t2.b + t1.d * t2.d;
        x = t1.x * t2.a + t1.y * t2.c + t2.x; y = t1.x * t2.b + t1.y * t2.d + t2.y;
    }

    // this = this * t;
    void postMultiply(const FMatrix2D &t)
    {
        FMatrix2D t0 = *this;
        multiply(t0, t);
    }

    // this = t * this
    void preMultiply(const FMatrix2D &t)
    {
        FMatrix2D t0 = *this;
        multiply(t, t0);
    }

    FVector2 transformPoint(const FVector2 &p) const
    {
        return FVector2(
            p.x * a + p.y * c + x,
            p.x * b + p.y * d + y);
    }

    FVector2 transformVector(const FVector2 &p) const
    {
        return FVector2(
            p.x * a + p.y * c,
            p.x * b + p.y * d);
    }
    
    
    FVector3 transformPoint(const FVector3 &p) const
    {
        return FVector3(
            p.x * a + p.z * c + x,
            p.y,
            p.x * b + p.z * d + y);
    }

    FVector3 transformVector(const FVector3 &p) const
    {
        return FVector3(
            p.x * a + p.z * c,
            p.y,
            p.x * b + p.z * d);
    }
};

NS_FXP_END
