//////////////////////////////////////////////////////////////////////
/// Desc  Color
/// Time  2020/12/09
/// Autor youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "common/FConfig.hpp"
#include <cstdint>

NS_FXP_BEGIN

class FXP_API Color
{
public:
    float r, g, b, a;

    Color()
        : r(0), g(0), b(0), a(0)
    {}

    Color(float _r, float _g, float _b, float _a = 1.0f)
        : r(_r), g(_g), b(_b), a(_a)
    {}

    Color(uint32_t hex)
    {
        fromHexARGB(hex);
    }

    Color operator * (float v)
    {
        float inv = 1.0f / v;
        return Color(r * inv, g * inv, b * inv, a * inv);
    }

    void fromHexARGB(uint32_t hex)
    {
        float inv = 1.0f / 255.0f;
        a = ((hex >> 24) & 0xff) * inv;
        r = ((hex >> 16) & 0xff) * inv;
        g = ((hex >> 8) & 0xff) * inv;
        b = ((hex >> 0) & 0xff) * inv;
    }

    void fromHexXRGB(uint32_t hex, float _a = 1.0f)
    {
        a = _a;

        float inv = 1.0f / 255.0f;
        r = ((hex >> 16) & 0xff) * inv;
        g = ((hex >> 8) & 0xff) * inv;
        b = ((hex >> 0) & 0xff) * inv;
    }

    static const Color red;
    static const Color green;
    static const Color blue;
    static const Color yellow;
    static const Color white;
    static const Color black;
    static const Color gray;
};

NS_FXP_END
