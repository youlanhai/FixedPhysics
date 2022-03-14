//////////////////////////////////////////////////////////////////////
/// Desc  FMath
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include "FFloat.hpp"
#include <cmath>

// 三角函数库。保证所有机器运算结果的一致性

extern const int LS_TABLE_COUNT;

extern const int LS_SIN_ENLARGE;
extern int LS_SIN_TABLE[];

extern const int LS_TAN_ENLARGE;
extern int LS_TAN_TABLE[];

NS_FXP_BEGIN

class FVector3;

class FXP_API FMath
{
public:
    static const FFloat PI;
    static const FFloat PI_FULL;
    static const FFloat PI_HALF;
    static const FFloat RADIAN_DEGREE;
    static const FFloat DEGREE_RADIAN;
    
    static const FFloat DEFAULT_EPSILON;

    /// pi / 180
    static const FFloat PI_y_180;
    /// 180 / pi
    static const FFloat _180_y_PI;

    /// 最小速度阀值。低于该值，认为是静止状态
    static const FFloat VECLOCITY_EPSILON;

    /// 最小的距离阈值。低于该值，认为位置重叠
    static const FFloat DISTANCE_EPSILON;

    static const FFloat FloatMax;

    static const FFloat FloatMin;

public:

    static uint32_t sqrt32(uint32_t a);

    static uint64_t sqrt64(uint64_t a);

    static int sqrt(int a)
    {
        if (a <= 0)
        {
            return 0;
        }

        return (int)sqrt32((uint32_t)a);
    }

    static int sqrt(int64_t a)
    {
        if (a <= 0L)
        {
            return 0;
        }

        if (a <= (int64_t)(0xffffffffLL))
        {
            return (int)sqrt32((uint32_t)a);
        }

        return (int)sqrt64((uint64_t)a);
    }

    static Fixed32 sqrt(Fixed32 a)
    {
        if (a.value <= 0)
        {
            return 0;
        }
        return Fixed32(true, sqrt(Fixed32::up64(a.value)));
    }

    static FFloat sqr(FFloat a)
    {
        return a * a;
    }

    static FFloat abs(FFloat a)
    {
        if (a < 0)
        {
            return -a;
        }
        return a;
    }

    template <typename T>
    static inline T min(T a, T b)
    {
        return a < b ? a : b;
    }

    template <typename T>
    static inline T max(T a, T b)
    {
        return a > b ? a : b;
    }

    template <typename T>
    static T min(T a, T b, T c)
    {
        return a < b ? (a < c ? a : c) : (b < c ? b : c);
    }

    template <typename T>
    static T max(T a, T b, T c)
    {
        return a > b ? (a > c ? a : c) : (b > c ? b : c);
    }

    template <typename T>
    static T clamp(T a, T low, T high)
    {
        return a > low ? (a < high ? a : high) : low;
    }

    template <typename T>
    static T clamp01(T a)
    {
        return clamp(a, T(0), T(1));
    }

    static FFloat sin(FFloat degree);
    static FFloat cos(FFloat degree);
    static FFloat tan(FFloat degree);

    static FFloat asin(FFloat value);
    static FFloat acos(FFloat value);
    static FFloat atan(FFloat value);
    static FFloat atan2(FFloat y, FFloat x);
    
    static bool almostEqual(FFloat v1, FFloat v2, FFloat epsilon = DEFAULT_EPSILON)
    {
        FFloat v = v1 - v2;
        return v >= -epsilon && v <= epsilon;
    }

    /// 将角度格式化到[0, 360)度范围内
    template<typename T>
    static T formatAngle(T angle)
    {
        while (angle < T(0))
        {
            angle += T(360);
        }
        while (angle >= T(360))
        {
            angle -= T(360);
        }
        return angle;
    }

    /// 将数值从src线性的变化(增加/减少)到dst
    static FFloat advanceValue(FFloat src, FFloat dst, FFloat delta);

    /// @brief 角度yaw转换为弧度值
    /// yaw是z轴的旋转角度，顺时针为正。
    /// radian是x轴的旋转角度，逆时针为正。
    static inline FFloat yaw2angle(FFloat yaw)
    {
        return FFloat(90) - yaw;
    }

    /// 弧度转换为角度yaw。@see yaw2radian
    static inline FFloat angle2yaw(FFloat angle)
    {
        return FFloat(90) - angle;
    }

    /// 将方向向量转换为角度yaw
    static inline FFloat dir2yaw(FFloat x, FFloat z)
    {
        return angle2yaw(atan2(z, x));
    }

    /// 将角度yaw转换为方向向量
    static inline void yaw2dir(FFloat yaw, FFloat &x, FFloat &z)
    {
        FFloat angle = yaw2angle(yaw);
        x = cos(angle);
        z = sin(angle);
    }

    /// 将角度yaw转换为方向向量
    static void yaw2dir(FFloat yaw, FVector3 &dir);
    static void angle2dir(FFloat angle, FVector3 &dir);
};

NS_FXP_END
