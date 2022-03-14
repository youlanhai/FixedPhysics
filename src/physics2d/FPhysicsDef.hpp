//////////////////////////////////////////////////////////////////////
/// Desc  FPhysicsDef
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once

#include "common/SmartPtr.hpp"
#include "math/FVector3.hpp"

NS_FXP_BEGIN
class FPhysics2D;
class FRigidbody;
class FCollider;

typedef SmartPtr<FRigidbody> FRigidbodyPtr;
typedef SmartPtr<FCollider> FColliderPtr;

/// 刚体类型。代表了cpBodyType
enum class FRigidbodyType
{
    /// 动态
    Dynamic,
    /// 动力学(手动)
    Kinematic,
    /// 静态
    Static,
};

enum class ShapeDataType
{
    sphere = 1,
    polygon = 2,
};

enum FColliderType
{
    FT_CIRCLE,
    FT_SEGMENT,
    FT_POLYGON,
};

/// 碰撞回调参数
struct Collision
{
    FRigidbody* rigidbody;
    FCollider* collider;
};

class FColliderFilter
{
public:
    /** 分组相等且不为0，则不碰撞。*/
    uint32_t        group = 0;
    uint32_t        layer = 0xffffffff;
    uint32_t        mask = 0xffffffff;

    bool canCollide(const FColliderFilter &other) const
    {
        if (group != 0 && group == other.group)
        {
            return false;
        }
        return (mask & other.layer) || (layer & other.mask);
    }
    
    void set(uint32_t group, uint32_t layer, uint32_t mask)
    {
        this->group = group;
        this->layer = layer;
        this->mask = mask;
    }
};


class FCollisionInfo
{
public:
    FCollider*      a = nullptr;
    FCollider*      b = nullptr;
    FVector3        normal;
    FFloat          distance = FFloat(0);

    FVector3        pointA;
    FVector3        pointB;

    /// 法线方向的分离力
    FFloat          forceNormal = FFloat(0);
    /// 切线方向的分离力
    FFloat          forceTangent = FFloat(0);

    FFloat          bias = FFloat(0);
    /// 法线方向质量系数
    FFloat          massNormal = FFloat(0);
    /// 切线方向的质量系数
    FFloat          massTangent = FFloat(0);

    int             hash = 0;
};

class FRaycastHit
{
public:
    FCollider*      collider = nullptr;
    FVector3        point;
    FVector3        normal;
    FFloat          distance = FFloat(0);
};

class FColliderPair
{
public:
    enum FCollisionState
    {
        STATE_ENTER,
        STATE_STAY,
        STATE_EXIT,
    };

    uint64_t        id = 0;
    FColliderPtr    a;
    FColliderPtr    b;
    int             stamp = 0;
    bool            isTrigger = false;
    FCollisionState state = STATE_ENTER;

    FCollisionInfo  collisionInfo;
};

NS_FXP_END
