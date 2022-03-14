//////////////////////////////////////////////////////////////////////
/// Desc  FRay
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "math/FVector2.hpp"

NS_FXP_BEGIN

class FXP_API FRay
{
public:
    FVector2 start;
    FVector2 end;
    FVector2 normal;
    FFloat distance;

    FRay()
    {}
    
    FRay(FVector2 _start, FVector2 _end)
        : start(_start)
        , end(_end)
    {
        normal = _end - _start;
        distance = normal.length();
        if (distance > 0)
        {
            normal /= distance;
        }
    }
    
    FRay(FVector2 _start, FVector2 _normal, FFloat _distance)
        : start(_start)
        , end(_start + _normal * _distance)
        , normal(_normal)
        , distance(_distance)
    {
    }
    
    void set(FVector2 _start, FVector2 _end)
    {
        start = _start;
        end = _end;
        normal = _end - _start;
        distance = normal.length();
        if (distance > 0)
        {
            normal /= distance;
        }
    }
    
    void set(FVector2 _start, FVector2 _normal, FFloat _distance)
    {
        start = _start;
        end = _start + _normal * _distance;
        normal = _normal;
        distance = _distance;
    }
};

NS_FXP_END
