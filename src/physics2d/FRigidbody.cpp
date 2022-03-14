//////////////////////////////////////////////////////////////////////
/// Desc  FRigidbody
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FRigidbody.hpp"
#include "FCollider.hpp"
#include "FPhysics2D.hpp"

#include "debug/LogTool.hpp"
#include <algorithm>

NS_FXP_BEGIN

FRigidbody* FRigidbody::New(FFloat mass, FFloat inertia)
{
    return new FRigidbody(mass, inertia);
}

FRigidbody* FRigidbody::NewStatic()
{
    return new FRigidbody(true);
}

FRigidbody* FRigidbody::NewKinematic()
{
    FRigidbody *r = new FRigidbody();
    r->setKinematic(true);
    return r;
}

FRigidbody::FRigidbody()
    : matrix(FMatrix2D::IDENTITY)
{
}

FRigidbody::FRigidbody(FFloat mass, FFloat inertia)
    : matrix(FMatrix2D::IDENTITY)
{
    setMass(mass);
    setInertia(inertia);
}

FRigidbody::FRigidbody(bool isStatic)
    : matrix(FMatrix2D::IDENTITY)
{
    setStatic(true);
}

FRigidbody::~FRigidbody()
{
    removeAllCollider();
}

void FRigidbody::setMass(FFloat m)
{
    mass = m;
    if (m >= FMath::FloatMax)
    {
        invMass = 0;
    }
    else if (m <= FFloat(0))
    {
        invMass = FMath::FloatMax;
    }
    else
    {
        invMass = FFloat(1) / m;
    }
}

void FRigidbody::setInertia(FFloat i)
{
    inertia = i;
    if (i >= FMath::FloatMax)
    {
        invInertia = 0;
    }
    else if (i <= FFloat(0))
    {
        invInertia = FMath::FloatMax;
    }
    else
    {
        invInertia = FFloat(1) / i;
    }
}

void FRigidbody::setType(FRigidbodyType type)
{
    if (type == type_)
    {
        return;
    }

    bool inPhysics = bInPhysics_;
    if (inPhysics)
    {
        physics_->removeRigidbody(this);
    }

    type_ = type;

    if (inPhysics)
    {
        physics_->addRigidbody(this);
    }
}

void FRigidbody::setBodyPosition(const FVector3 &v)
{
    if (position == v)
    {
        return;
    }
    
    position = v;
    markTransformDirty();
}

void FRigidbody::setBodyAngle(FFloat v)
{
    if (angle == v)
    {
        return;
    }
    
    angle = v;
    markTransformDirty();
}

void FRigidbody::setBodyScale(FFloat v)
{
    if (scale == v)
    {
        return;
    }
    
    scale = v;
    markTransformDirty();
}

void FRigidbody::update(FFloat dt)
{
    if (!isDynamic())
    {
        return;
    }
    
    // 计算速度衰减
    velocity *= physics_->getDamping();
    angleVelocity *= physics_->getDamping();

    // 计算作用力. v += a * t; a = F / m
    velocity += (force + physics_->getGravity()) * invMass * dt;
    angleVelocity += torque * invInertia * dt;

    // 计算脉冲力
    velocity += forceImpulse * invMass;
    angleVelocity += torqueImpulse * invInertia;

    forceImpulse = FVector3::Zero;
    torqueImpulse = FFloat(0);
}

void FRigidbody::postUpdate(FFloat dt)
{
    if (isDynamic())
    {
        position += (velocity + pulseVelocity) * dt;
        angle += (angleVelocity + pulseAngleVelocity) * dt;

        transformDirty_ = true;

        pulseVelocity = FVector3::Zero;
        pulseAngleVelocity = FFloat(0);

        FFloat sleepThreshold = physics_->getIdleSpeedThreshold();
        if (velocity.lengthSq() <= sleepThreshold * sleepThreshold &&
            angleVelocity <= sleepThreshold * 10)
        {
            idleTime += dt;
            if (idleTime > physics_->getSleepTimeThreshold())
            {
                velocity = FVector3::Zero;
                angleVelocity = 0;
            }
        }
        else
        {
            idleTime = FFloat(0);
        }
    }

    if (transformDirty_)
    {
        updateTransform();
    }
}

void FRigidbody::updateTransform()
{
    transformDirty_ = false;
    matrix.setTransform(FVector2(position.x, position.z), angle, FVector2(scale, scale));

    for (auto collider : colliders_)
    {
        FBB bb = collider->getBounds();
        collider->updateTransform();

        if (collider->isInPhysics() && bb != collider->getBounds())
        {
            physics_->onColliderBBChange(collider.get());
        }
    }
}

bool FRigidbody::canSleep()
{
    if (isStatic() || !isActive_)
    {
        return true;
    }

    if (collisionStamp_ == physics_->getTickStamp())
    {
        return false;
    }

    return idleTime > physics_->getSleepTimeThreshold();
}

FFloat FRigidbody::getPointMoment(const FVector3 & point, const FVector3 & normal)
{
    if (isStatic())
    {
        return FFloat(0);
    }
    
    FVector3 r = point - position;
    FFloat rn = r.dot(normal);
    return invMass + invInertia * (r.lengthSq() - rn * rn);
}

/// 获得点的速度
FVector3 FRigidbody::getPointVelocity(const FVector3& point)
{
    // 点的旋转速度为：每秒转过的弧度L = (angle / 360) * 2 PI * R。
    // 点的旋转方向为：r的切线方向
    FVector3 r = point - position;
    return velocity + FVector3(-r.z, FFloat(0), r.x) * angleVelocity * FMath::DEGREE_RADIAN;
}

void FRigidbody::setActive(bool active)
{
    if (physics_ == nullptr || !bInPhysics_ || active == isActive_)
    {
        return;
    }
    
    isActive_ = active;
    if (active)
    {
        idleTime = FFloat(0);
        physics_->addActiveRigidbody(this);
    }
}

void FRigidbody::addCollider(FCollider * collider)
{
    if (collider->rigidbody_ != nullptr)
    {
        //LOG_ERROR("collider already added to other rigidbody");
        return;
    }
    
    collider->rigidbody_ = this;
    colliders_.push_back(collider);

    if (bInPhysics_)
    {
        physics_->addCollider(collider);
    }
}

void FRigidbody::removeCollider(FCollider *collider)
{
    if (collider->rigidbody_ != this)
    {
        LOG_ERROR("collider not added to this rigidbody");
        return;
    }
    
    if (bInPhysics_)
    {
        physics_->removeCollider(collider);
    }

    collider->rigidbody_ = nullptr;
    colliders_.erase(std::remove(colliders_.begin(), colliders_.end(), collider), colliders_.end());
}

void FRigidbody::removeAllCollider()
{
    if (bInPhysics_)
    {
        for (auto collider : colliders_)
        {
            physics_->removeCollider(collider.get());
        }
    }

    for (auto collider : colliders_)
    {
        collider->rigidbody_ = nullptr;
    }
    
    colliders_.clear();
}

void FRigidbody::setPhysics(FPhysics2D *physics)
{
    if (physics == physics_)
    {
        return;
    }
    
    physics_ = physics;
    
    if (physics_)
    {
        id_ = physics_->allocateID();
    }
}

void FRigidbody::onAddToPhysicsWorld()
{
    updateTransform();

    bInPhysics_ = true;
    for(auto& collider : colliders_)
    {
        physics_->addCollider(collider.get());
    }

    setActive(true);
}

void FRigidbody::onRemoveFromPhysicsWorld()
{
    bInPhysics_ = false;
    isActive_ = false;
    for(auto& collider : colliders_)
    {
        physics_->removeCollider(collider.get());
    }
}

size_t FRigidbody::getMemorySize()
{
    size_t v = sizeof(*this) + colliders_.capacity() * sizeof(FCollider*);

    for (auto &collider : colliders_)
    {
        v += collider->getMemorySize();
    }

    return v;
}

void FRigidbody::addForce(const FVector3 &force, const FVector3 &point)
{
    this->force += force;
}

NS_FXP_END
