//////////////////////////////////////////////////////////////////////
/// Desc  FRigidbody
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "math/FVector3.hpp"
#include "math/FMatrix2D.hpp"
#include "math/FMath.hpp"
#include "common/SmartPtr.hpp"
#include "common/IRefCount.hpp"
#include "FPhysicsDef.hpp"

#include <vector>

NS_FXP_BEGIN
class FPhysics2D;
class FCollider;

class FXP_API FRigidbody : public IRefCount
{
public:
    /// 创建动态刚体
    static FRigidbody* New(FFloat mass = FFloat(1), FFloat inertia = FMath::FloatMax);
    /// 创建静态刚体
    static FRigidbody* NewStatic();
    /// 创建动力学刚体
    static FRigidbody* NewKinematic();
    
    FRigidbody();
    FRigidbody(FFloat mass, FFloat inertia);
    explicit FRigidbody(bool isStatic);
    ~FRigidbody();
    
    void updateTransform();

    /// 设置质量
    void setMass(FFloat m);

    /// 获取质量
    FFloat getMass() { return mass; }
    
    void setInertia(FFloat i);
    FFloat getInertia() const { return inertia; }

    /// 获取当前所属的物理对象
    FPhysics2D* getPhysics(){ return physics_; }
    /// 内部方法
    void setPhysics(FPhysics2D *physics);

    /// 设置刚体的类型。静态、动态或者动力学 @see RigidbodyType
    void setType(FRigidbodyType type);
    /// 获取刚体的类型。@see setType, RigidbodyType
    FRigidbodyType getType() const { return type_; }
    
    uint32_t getID() const { return id_; }
    
    /// 设置为静态刚体
    void setStatic(bool v){ setType(v ? FRigidbodyType::Static : FRigidbodyType::Dynamic); }
    /// 是否是静态刚体
    bool isStatic() const { return getType() == FRigidbodyType::Static; }
    
    /// 设置为动态刚体
    void setDynamic(bool v){ setType(v ? FRigidbodyType::Dynamic : FRigidbodyType::Static); }
    /// 是否是动态刚体
    bool isDynamic() const { return getType() == FRigidbodyType::Dynamic; }
    
    /// 设置为动力学刚体
    void setKinematic(bool v){ setType(v ? FRigidbodyType::Kinematic : FRigidbodyType::Dynamic); }
    /// 是否是动力学刚体
    bool isKinematic() const { return getType() == FRigidbodyType::Kinematic; }

    bool canSleep();

    void setActive(bool active);
    bool isActive() const { return isActive_;}

    /** 是否已加入到物理世界中 */
    bool isInPhysics() const { return bInPhysics_; }
    
    /// 设置休眠状态
    void setSleep(bool sleep){ setActive(!sleep); }
    /// 是否在休眠中
    bool isSleeping() const { return !isActive_; }
    
    /// 设置力
    void setForce(const FVector3 &force) { this->force = force; setActive(true); }
    /// 获取力
    const FVector3& getForce() { return force; }
    
    /// 给某个位置添加。lua层，point可省略，默认为(0, 0)
    void addForce(const FVector3 &force, const FVector3 &point = FVector3::Zero);
    
    /// 获取所管理的碰撞体数量
    size_t getNumColliders() const { return colliders_.size(); }
    /// 获取所管理的碰撞体对象
    FCollider* getCollider(size_t i){ return colliders_[i].get(); }
    
  
    void setBodyPosition(const FVector3 &v);
    const FVector3& getBodyPosition() const { return position; }
    
    void setBodyAngle(FFloat v);
    FFloat getBodyAngle() const{ return angle; }
    
    void setBodyScale(FFloat scale);
    FFloat getBodyScale() const { return scale; }
    
    void setBodyVelocity(const FVector3 &v){ velocity = v; setActive(true); }
    const FVector3& getBodyVelocity() const { return velocity; }
    
    void setAngleVelociy(FFloat v) { angleVelocity = v; setActive(true); }
    FFloat getAngleVelocity() const { return angleVelocity; }
 
    virtual size_t getMemorySize();
    
    const FMatrix2D& getMatrix() const;

    /// 添加collider
    void addCollider(FCollider *collider);
    /// 移除collider
    void removeCollider(FCollider *collider);
    void removeAllCollider();
    
private:
    friend class FPhysics2D;
    friend class FCollider;

    /// @private 添加到物理世界后回调
    void onAddToPhysicsWorld();
    /// @private 从物理世界删除后回调
    void onRemoveFromPhysicsWorld();

    FFloat getPointMoment(const FVector3& point, const FVector3 &normal);

    /// 获得点的速度
    FVector3 getPointVelocity(const FVector3& point);

    void applyImpulse(const FVector3& force);

    void applyTorqueImpulse(const FVector3& point, const FVector3& torque);
    
    bool isTransformDirty() { return transformDirty_; }
    void markTransformDirty();
    
    /** 更新坐标 */
    void update(FFloat deltaTime);
    void postUpdate(FFloat deltaTime);
    
    void setCollisionStamp(int index) { collisionStamp_ = index; }
    int getCollisionStamp() const { return collisionStamp_; }

private:
    uint32_t        id_ = 0;
    
    // 质量
    FFloat          mass = 1;
    FFloat          invMass = FFloat(1);

    // 角动量
    FFloat          inertia = FFloat(1);
    FFloat          invInertia = FFloat(1);
    
    // 位置
    FVector3        position;
    // 旋转角度。单位: 度
    FFloat          angle;
    // 缩放
    FFloat          scale = FFloat(1);
    
    FVector3        velocity;
    FVector3        force;
    FVector3        forceImpulse;

    // 角速度
    FFloat          angleVelocity;
    // 力矩。每秒旋转加速度
    FFloat          torque;
    FFloat          torqueImpulse;

    /** 瞬间速度。仅作用一帧 */
    FVector3        pulseVelocity;
    /** 瞬间角速度。仅作用一帧 */
    FFloat          pulseAngleVelocity;

    FFloat          idleTime = FFloat(0);

    FMatrix2D       matrix;

    FPhysics2D*     physics_ = nullptr;
    bool            isActive_ = false;
    bool            bInPhysics_ = false;
    bool            transformDirty_ = false;
    
    FRigidbodyType  type_ = FRigidbodyType::Dynamic;

    std::vector<SmartPtr<FCollider>>   colliders_;
    /** 用来标记发生碰撞的帧索引 */
    int             collisionStamp_ = 0;
};

inline const FMatrix2D& FRigidbody::getMatrix() const
{
    if (transformDirty_)
    {
        const_cast<FRigidbody*>(this)->updateTransform();
    }
    return matrix;
}

inline void FRigidbody::applyImpulse(const FVector3& force)
{
    if (isStatic())
    {
        return;
    }
    velocity += force * invMass;
}

inline void FRigidbody::applyTorqueImpulse(const FVector3& point, const FVector3& torque)
{
    if (isStatic())
    {
        return;
    }
    FVector3 radius = point - position;
    angleVelocity += radius.crossXZ(torque) * FMath::RADIAN_DEGREE * invInertia;
}

inline void FRigidbody::markTransformDirty()
{
    transformDirty_ = true;
    setActive(true);
}

NS_FXP_END
