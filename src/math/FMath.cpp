//////////////////////////////////////////////////////////////////////
/// Desc  FMath
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FMath.hpp"
#include "FVector3.hpp"

#include <cassert>
#include <algorithm>

NS_FXP_BEGIN

const FFloat FMath::PI = FFloat(true, 3217); // 3142
const FFloat FMath::PI_FULL = FMath::PI;
const FFloat FMath::PI_HALF = FMath::PI / FFloat(2);
const FFloat FMath::RADIAN_DEGREE = FMath::_180_y_PI;
const FFloat FMath::DEGREE_RADIAN = FMath::PI_y_180;

const FFloat FMath::DEFAULT_EPSILON(true, 1);

/// pi / 180
const FFloat FMath::PI_y_180 = PI_FULL / FFloat(180);
/// 180 / pi
const FFloat FMath::_180_y_PI = FFloat(180) / PI_FULL;

/// 最小速度阀值。低于该值，认为是静止状态
const FFloat FMath::VECLOCITY_EPSILON(true, 10);

/// 最小的距离阈值。低于该值，认为位置重叠
const FFloat FMath::DISTANCE_EPSILON(true, 10);

// const FFloat FMath::FloatMax = FFloat::MAX; 静态变量存在先后初始化问题，不可控
const FFloat FMath::FloatMax(true, Fixed32::V_MAX);

const FFloat FMath::FloatMin(true, Fixed32::V_MIN);

// 这里的angle只是一个倍率概念，跟真实的角度没有关系
static const int ANGLE_90 = LS_TABLE_COUNT;
static const int ANGLE_180 = ANGLE_90 * 2;
static const int ANGLE_270 = ANGLE_90 * 3;
static const int ANGLE_360 = ANGLE_90 * 4;

static inline int degree2angle(FFloat degree)
{
    int angle = int(degree * ANGLE_90 / FFloat(90)) % ANGLE_360;
    if (angle < 0)
    {
        angle = ANGLE_360 + angle;
    }
    return angle;
}

static inline FFloat angle2degree(int angle)
{
    return angle * FFloat(90) / ANGLE_90;
}

static inline FFloat sinToFloat(int value)
{
    return FFloat(true, int(FFloat::up64(value) / LS_SIN_ENLARGE));
}

static inline int sinFromFloat(FFloat value)
{
    return FFloat::down32(int64_t(value.value) * LS_SIN_ENLARGE);
}

static inline FFloat tanToFloat(int value)
{
    return FFloat(true, int(FFloat::up64(value) / LS_TAN_ENLARGE));
}

static inline int tanFromFloat(FFloat value)
{
    return FFloat::down32(int64_t(value.value) * LS_TAN_ENLARGE);
}

// 0 < x < 90
// sin(x + 90) = cos(x) = sin(90 - x)
// sin(x + 180) = -sin(x)
// sin(x + 270) = -sin(90 - x)
FFloat FMath::sin(FFloat degree)
{
    assert(LS_SIN_ENLARGE >= FFloat::PRECISION && "SIN_ENLARGE should >= FFloat precision");
    int angle = degree2angle(degree);
    int ret = 0;

    if (angle < ANGLE_90)
    {
        ret = LS_SIN_TABLE[angle];
    }
    else if (angle < ANGLE_180)
    {
        angle -= ANGLE_90;
        ret = LS_SIN_TABLE[ANGLE_90 - angle];
    }
    else if (angle < ANGLE_270)
    {
        angle -= ANGLE_180;
        ret = -LS_SIN_TABLE[angle];
    }
    else if (angle < ANGLE_360)
    {
        angle -= ANGLE_270;
        ret = -LS_SIN_TABLE[ANGLE_90 - angle];
    }
    return sinToFloat(ret);
}

// 0 < x < 90
// cos(x) = sin(90 - x)
// cos(x + 90) = -sin(x)
// cos(x + 180) = -cos(x) = -sin(90 - x)
// cos(x + 270) = sin(x)
FFloat FMath::cos(FFloat degree)
{
    int angle = degree2angle(degree);
    int ret = 0;

    if (angle < ANGLE_90)
    {
        ret = LS_SIN_TABLE[ANGLE_90 - angle];
    }
    else if (angle < ANGLE_180)
    {
        angle -= ANGLE_90;
        ret = -LS_SIN_TABLE[angle];
    }
    else if (angle < ANGLE_270)
    {
        angle -= ANGLE_180;
        ret = -LS_SIN_TABLE[ANGLE_90 - angle];
    }
    else if (angle < ANGLE_360)
    {
        angle -= ANGLE_270;
        ret = LS_SIN_TABLE[angle];
    }
    return sinToFloat(ret);
}

// 0 < x < 90
// tan(x + 90) = -cot(x) = -tan(90 - x)
// tan(x + 180) = tan(x)
// tan(x + 270) = -tan(90 - x)
FFloat FMath::tan(FFloat degree)
{
    assert(LS_TAN_ENLARGE >= FFloat::PRECISION && "TAN_ENLARGE should >= FFloat precision");
    int angle = degree2angle(degree);
    int ret = 0;

    if (angle <= ANGLE_90)
    {
        ret = LS_TAN_TABLE[angle];
    }
    else if (angle <= ANGLE_180)
    {
        angle -= ANGLE_90;
        ret = -LS_TAN_TABLE[ANGLE_90 - angle];
    }
    else if (angle <= ANGLE_270)
    {
        angle -= ANGLE_180;
        ret = LS_TAN_TABLE[angle];
    }
    else if (angle <= ANGLE_360)
    {
        angle -= ANGLE_270;
        ret = -LS_TAN_TABLE[ANGLE_90 - angle];
    }
    return tanToFloat(ret);
}


// 查找最接近的索引值。用于反三角函数
static int findTableIndex(int *table, int v)
{
    int l = 0;
    int r = LS_TABLE_COUNT; // - 1。把值扩充到开区间
    int m, t;

    while (l < r)
    {
        m = (l + r) / 2;
        t = table[m];
        if (v == t)
        {
            l = r = m;
        }
        else if (v < t)
        {
            r = m - 1;
        }
        else
        {
            l = m + 1;
        }
    }

    return l < r ? l : r;
}

FFloat FMath::asin(FFloat value)
{
    int v = sinFromFloat(value);
    int angle = findTableIndex(LS_SIN_TABLE, std::abs(v));
    FFloat radian = angle2degree(angle);
    if (v < 0)
    {
        radian = -radian;
    }
    return radian;
}

FFloat FMath::acos(FFloat value)
{
    int v = sinFromFloat(value);
    int angle = findTableIndex(LS_SIN_TABLE, std::abs(v));
    FFloat radian = angle2degree(angle);
    if (v < 0)
    {
        radian = FFloat(90) + radian;
    }
    else
    {
        radian = FFloat(90) - radian;
    }
    return radian;
}

FFloat FMath::atan(FFloat value)
{
    int v = tanFromFloat(value);

    int angle = findTableIndex(LS_TAN_TABLE, std::abs(v));
    FFloat radian = angle2degree(angle);
    if (value < 0)
    {
        radian = -radian;
    }
    return radian;
}

FFloat FMath::atan2(FFloat y, FFloat x)
{
    FFloat radian;
    if (x != 0)
    {
        radian = abs(atan(y / x));
    }
    else
    {
        radian = FFloat(90);
    }

    if (x < 0)
    {
        if (y < 0)
        {
            // 三象限
            radian = radian - FFloat(180);
        }
        else
        {
            // 二象限
            radian = FFloat(180) - radian;
        }
    }
    else
    {
        if (y < 0)
        {
            // 四象限
            radian = -radian;
        }
        else
        {
            // 一象限
        }
    }
    return radian;
}

// https://www.zhihu.com/question/35122102
uint32_t FMath::sqrt32(uint32_t a)
{
    uint32_t num = 0u;
    uint32_t num2 = 0u;
    for (size_t i = 0; i < 16; i++)
    {
        num2 <<= 1;
        num <<= 2;
        num += a >> 30;
        a <<= 2;
        if (num2 < num)
        {
            num2 += 1u;
            num -= num2;
            num2 += 1u;
        }
    }

    return (num2 >> 1) & 0xffff;
}

uint64_t FMath::sqrt64(uint64_t a)
{
    uint64_t num = 0;
    uint64_t num2 = 0;
    for (size_t i = 0; i < 32; i++)
    {
        num2 <<= 1;
        num <<= 2;
        num += a >> 62;
        a <<= 2;
        if (num2 < num)
        {
            num2 += 1uL;
            num -= num2;
            num2 += 1uL;
        }
    }

    return (num2 >> 1) & 0xffffffffu;
}

void FMath::yaw2dir(FFloat yaw, FVector3 &dir)
{
    yaw2dir(yaw, dir.x, dir.z);
    dir.y = FFloat(0);
}

void FMath::angle2dir(FFloat angle, FVector3 &dir)
{
    dir.set(cos(angle), FFloat(0), sin(angle));
}

FFloat FMath::advanceValue(FFloat src, FFloat dst, FFloat delta)
{
    if (src < dst)
    {
        return std::min(src + delta, dst);
    }
    else if (src > dst)
    {
        return std::max(src - delta, dst);
    }
    else
    {
        return dst;
    }
}

NS_FXP_END
