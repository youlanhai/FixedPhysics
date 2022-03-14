//////////////////////////////////////////////////////////////////////
/// Desc  Fixed32
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include <cstdint>
#include "common/FConfig.hpp"

NS_FXP_BEGIN

/** 定点数 */
class FXP_API Fixed32
{
public:
    static const int SHIFT = 10;
    static const uint32_t LOW_MASK = (1 << SHIFT) - 1;

    static const int PRECISION = 1 << SHIFT;
    static const float INV_PRECISION;

    static const int V_MIN = 0x80000000;
    static const int V_MAX = 0x7fffffff;

    int value;

    Fixed32() : value(0) {}
    Fixed32(const Fixed32 &v) : value(v.value) {}

    // TODO 研究一下raw作为第一个参数，是否会引起传参时没有利用到寄存器
    Fixed32(bool raw, int v) : value(v) {}

    Fixed32(int i) { setInt(i); }
    Fixed32(int64_t i) { setInt(int(i)); }

#if defined(LS_ANDROID) && defined(__aarch64__)
    Fixed32(long long i) { setInt(int(i)); }
#endif

    /** 使用整数部分和浮点数部分来构造定点数
     *  @param iPart    整数位
     *  @param f10      浮点数10分位
     *  @param f100     浮点数100分位
     *  @param f1000    浮点数1000分位
     */
    Fixed32(int iPart, int f0, int f100 = 0, int f1000 = 0)
    {
        int fPart = f0 * 100 + f100 * 10 + f1000;
        value = (iPart << SHIFT) + (fPart << SHIFT) / 1000;
    }

    explicit Fixed32(float v) { setFloat(v); }
    explicit Fixed32(double v) { setDouble(v); }

    explicit operator int() const { return value >> SHIFT; }
    explicit operator float() const { return asFloat(); }
    explicit operator double() const { return asDouble(); }

    const Fixed32& operator *= (const Fixed32 &v) { value = int(int64_t(value) * v.value >> SHIFT); return *this; }
    const Fixed32& operator /= (const Fixed32 &v) { value = int((int64_t(value) << SHIFT) / v.value); return *this; }
    const Fixed32& operator += (const Fixed32 &v) { value += v.value; return *this; }
    const Fixed32& operator -= (const Fixed32 &v) { value -= v.value; return *this; }

    Fixed32 operator - () const { return Fixed32(true, -value); }

    Fixed32 operator ++ ();
    Fixed32 operator ++ (int);
    Fixed32 operator -- ();
    Fixed32 operator -- (int);

    Fixed32 operator = (const Fixed32 & v) { value = v.value; return *this; }
    Fixed32 operator = (int v) { value = v << SHIFT; return *this; }

    Fixed32 operator = (float v) = delete;
    Fixed32 operator = (double v) = delete;

    inline void setInt(int v) { value = v << SHIFT; }
    inline void setFloat(float v) { value = int(v * PRECISION); }
    inline void setDouble(double v) { value = int(v * PRECISION); }

    inline int asInt() const { return value >> SHIFT; }
    inline float asFloat() const { return value * INV_PRECISION; }
    inline double asDouble() const { return value * double(INV_PRECISION); }

    inline bool isInt() const { return (value & LOW_MASK) == 0; } // value % 1000 == 0

    static inline int up32(int v){ return v << SHIFT; }
    static inline int64_t up64(int v) { return int64_t(v) << SHIFT; }
    static inline int64_t up64(int64_t v){ return v << SHIFT; }
    static inline uint64_t up64(uint64_t v) { return v << SHIFT; }

    static inline int64_t down(int64_t v){ return v >> SHIFT; }
    static inline uint64_t down(uint64_t v) { return v >> SHIFT; }
    static inline int down(int v){ return v >> SHIFT; }
    static inline int down32(int64_t v){ return int(v >> SHIFT); }
    static inline int down32(int v){ return v >> SHIFT; }
};

inline Fixed32 Fixed32::operator ++ ()
{
    value += PRECISION;
    return *this;
}

inline Fixed32 Fixed32::operator ++ (int)
{
    Fixed32 old = *this;
    value += PRECISION;
    return old;
}

inline Fixed32 Fixed32::operator -- ()
{
    value -= PRECISION;
    return *this;
}

inline Fixed32 Fixed32::operator -- (int)
{
    Fixed32 old = *this;
    value -= PRECISION;
    return old;
}

inline Fixed32 operator *  (const Fixed32 &a, const Fixed32 &b) { return Fixed32(true, Fixed32::down32(int64_t(a.value) * b.value)); }
inline Fixed32 operator /  (const Fixed32 &a, const Fixed32 &b) { return Fixed32(true, int(Fixed32::up64(a.value) / b.value)); }
inline Fixed32 operator +  (const Fixed32 &a, const Fixed32 &b) { return Fixed32(true, a.value + b.value); }
inline Fixed32 operator -  (const Fixed32 &a, const Fixed32 &b) { return Fixed32(true, a.value - b.value); }
inline bool operator <  (const Fixed32 &a, const Fixed32 &b) { return a.value < b.value; }
inline bool operator >  (const Fixed32 &a, const Fixed32 &b) { return a.value > b.value; }
inline bool operator == (const Fixed32 &a, const Fixed32 &b) { return a.value == b.value; }
inline bool operator != (const Fixed32 &a, const Fixed32 &b) { return a.value != b.value; }
inline bool operator <= (const Fixed32 &a, const Fixed32 &b) { return a.value <= b.value; }
inline bool operator >= (const Fixed32 &a, const Fixed32 &b) { return a.value >= b.value; }


/** 转换为1000进制的int */
inline int toi(const Fixed32 &v){ return Fixed32::down32(int64_t(v.value) * 1000); }

inline float tof(const Fixed32 &v){ return v.asFloat(); }

NS_FXP_END
