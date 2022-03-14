//////////////////////////////////////////////////////////////////////
/// Desc  Physics2D
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FPhysics2D.hpp"
#include "FBVHTree.hpp"
#include "FRigidbody.hpp"
#include "FCollider.hpp"
#include "FGJK.hpp"
#include "debug/DebugDraw.hpp"
#include "debug/LogTool.hpp"
#include "debug/Profiler.hpp"

#include <algorithm>
#include <cassert>

DEFINE_LOG_COMPONENT(LOG_LEVEL_DEBUG, "Physics2D");

NS_FXP_BEGIN

typedef bool(*CollisionMethod)(FCollider *a, FCollider *b, FCollisionInfo &info);

static FVector2 getPenetrateNormalByVelocity(FRigidbody *a, FRigidbody *b)
{
    FVector2 normal = (a->getBodyVelocity() - b->getBodyVelocity()).toXZ();
    if (normal.isZero())
    {
        normal = FVector2(FFloat(1), FFloat(0));
    }
    normal.normalize();
    return normal;
}

static void swapCollisionInfo(FCollisionInfo &contact)
{
    std::swap(contact.a, contact.b);
    std::swap(contact.pointA, contact.pointB);
    contact.normal = -contact.normal;
}

static bool testCircleAndCircle(FCircleCollider *a, FCircleCollider *b, FCollisionInfo &info)
{
    FVector2 dir = b->getWorldCenter() - a->getWorldCenter();
    FFloat distanceSq = dir.lengthSq();
    FFloat radius = a->getWorldRadius() + b->getWorldRadius();

    if (distanceSq > radius * radius)
    {
        return false;
    }

    FFloat distance = FMath::sqrt(distanceSq);
    FVector2 normal;
    
    if (distance == 0)
    {
        // 圆心重合
        normal = getPenetrateNormalByVelocity(a->getRigidbody(), b->getRigidbody());
    }
    else
    {
        // 单位化
        normal = dir / distance;
    }

    info.distance = radius - distance;
    info.normal.setXZ(normal);
    info.pointA.setXZ(a->getWorldCenter() + normal * a->getWorldRadius());
    info.pointB.setXZ(b->getWorldCenter() - normal * b->getWorldRadius());
    return true;
}

static bool testSegmentAndSegment(FSegmentCollider *ca, FSegmentCollider *cb, FCollisionInfo &info)
{
    const FVector2& a = ca->getWorldEnd() - ca->getWorldStart();
    const FVector2& b = cb->getWorldEnd() - cb->getWorldStart();
    FVector2 c = cb->getWorldStart() - ca->getWorldStart();

    FFloat denominator = a.x * b.y - a.y * b.x;
    if (denominator == 0)
    {
        return false;
    }

    FFloat t1 = (c.x * b.y - c.y * b.x) / denominator;
    FFloat t2 = (c.x * a.y - c.y * a.x) / denominator;
    if (t1 < FFloat(0) || t1 > FFloat(1) ||
        t2 < FFloat(0) || t2 > FFloat(1))
    {
        return false;
    }

    info.distance = a.length() * t1;
    info.normal.setXZ(ca->getWorldNormal());
    info.pointA.setXZ(ca->getWorldStart() + ca->getWorldNormal() * info.distance);
    info.pointB = info.pointA;
    return true;
}

static bool testSegmentAndCircle(FSegmentCollider *a, FCircleCollider *b, FCollisionInfo &info)
{
    FVector2 AB = a->getWorldEnd() - a->getWorldStart();
    FVector2 AC = b->getWorldCenter() - a->getWorldStart();
    
    FFloat lengthABSq = AB.lengthSq();
    // 线段长度不合法
    if (lengthABSq == 0)
    {
        return false;
    }

    // projection = dot(AB, AC) / |AB|
    // 将投影映射到AB的[0, 1]区间
    // ratio = projection / |AB| = dot(AB, AC) / |AB|^2
    FFloat ratio = FMath::clamp01(AB.dot(AC) / lengthABSq);
    FVector2 nearstPoint = a->getWorldStart() + AB * ratio;

    FFloat radius = b->getWorldRadius();

    FVector2 dir = b->getWorldCenter() - nearstPoint;
    FFloat distanceSq = dir.lengthSq();

    if (distanceSq > radius * radius)
    {
        return false;
    }

    FFloat distance = FMath::sqrt(distanceSq);
    if (distance == 0)
    {
        // 圆心重合
        info.normal.setXZ(getPenetrateNormalByVelocity(a->getRigidbody(), b->getRigidbody()));
        info.pointA.setXZ(nearstPoint);
        info.pointB.setXZ(b->getWorldCenter());
        info.distance = radius;
    }
    else
    {
        FVector2 normal = dir / distance;
        // 单位化
        info.normal.setXZ(normal);
        info.pointA.setXZ(nearstPoint);
        info.pointB.setXZ(b->getWorldCenter() - normal * b->getWorldRadius());
        info.distance = radius - distance;
    }
    return true;
}

static bool testWithGJK(FCollider *a, FCollider *b, FCollisionInfo &info)
{
    LS_PROFILER(PK_PHYSICS_GJK_TEST);

    FGJK *gjk = a->getPhysics()->getGJK();
    if (!gjk->queryCollision(a, b))
    {
        return false;
    }

    info.distance = gjk->penetrationDistance;
    info.normal.setXZ(gjk->penetrationNormal);

    info.pointA.setXZ(gjk->closestOnA);
    info.pointB.setXZ(gjk->closestOnB);
    return true;
}

static CollisionMethod collisionTestMethods[3][3] = {
    //          circle                               segment                         polygon
    /*circle */ {(CollisionMethod)testCircleAndCircle, nullptr, nullptr},
    /*segment*/ {(CollisionMethod)testSegmentAndCircle, (CollisionMethod)testSegmentAndSegment, nullptr},
    /*polygon*/ {(CollisionMethod)testWithGJK, (CollisionMethod)testWithGJK, (CollisionMethod)testWithGJK},
};


static bool collisionTest(FCollider *a, FCollider *b, FCollisionInfo &info)
{
    LS_PROFILER(PK_PHYSICS_COLLISION_TEST);
    if (a->getType() < b->getType())
    {
        std::swap(a, b);
    }

    info.a = a;
    info.b = b;
    CollisionMethod method = collisionTestMethods[a->getType()][b->getType()];
    return method(a, b, info);
}

FPhysics2D::FPhysics2D()
{
    dynamicTree_ = new FBVHTree();
    staticTree_ = new FBVHTree();
    gjk_ = new FGJK();

    staticRigidbody_ = new FRigidbody(true);
    staticRigidbody_->setPhysics(this);
}

FPhysics2D::~FPhysics2D()
{
    enableHandle_ = false;

    clear();

    delete dynamicTree_;
    dynamicTree_ = nullptr;

    delete staticTree_;
    staticTree_ = nullptr;

    staticRigidbody_ = nullptr;
}

void FPhysics2D::init()
{
    addRigidbody(staticRigidbody_.get());
}

void FPhysics2D::clear()
{
    idCounter = 0;
    tickStamp = 0;

    dynamicTree_->clear();
    staticTree_->clear();

    activeBodies_.clear();
    colliderPairs_.clear();

    for (auto rigidbody : rigidbodys_)
    {
        rigidbody->isActive_ = false;
    }
    rigidbodys_.clear();

    staticRigidbody_->removeAllCollider();
}

void FPhysics2D::tick(FFloat deltaTime)
{
    Profiler::getDefault()->begin(PK_PHYSICS_TICK); // 始终统计
    ++tickStamp;
    
    // 更新刚体运动属性
    for (auto& pair : activeBodies_)
    {
        pair.second->update(deltaTime);
        if (pair.second->isTransformDirty())
        {
            pair.second->updateTransform();
        }
    }

    LS_PROFILER_BEGIN(PK_PHYSICS_BVH_REBUILD);
    if (staticTree_->getChangedCount() > rebuildTreeThreshold_)
    {
        staticTree_->rebuild();
    }
    if (dynamicTree_->getChangedCount() > rebuildTreeThreshold_)
    {
        dynamicTree_->rebuild();
    }
    LS_PROFILER_END(PK_PHYSICS_BVH_REBUILD);

    queryColliderPairs();

    LS_PROFILER_BEGIN(PK_PHYSICS_NOTIFY);
    // 更新碰撞对
    std::vector<uint64_t> pendingRemove;
    for (auto &pair : colliderPairs_)
    {
        updateColliderPair(deltaTime, pair.second);
        if (pair.second.state == FColliderPair::STATE_EXIT)
        {
            pendingRemove.push_back(pair.first);
        }
    }
    LS_PROFILER_END(PK_PHYSICS_NOTIFY);

    for (uint64_t id : pendingRemove)
    {
        auto it = colliderPairs_.find(id);
        if (it != colliderPairs_.end())
        {
            colliderPairs_.erase(it);
        }
    }

    LS_PROFILER_BEGIN(PK_PHYSICS_PRE_SEPERATION);
    for (auto &pair : colliderPairs_)
    {
        if (!pair.second.isTrigger)
        {
            doPreSeperation(deltaTime, pair.second);
        }
    }
    LS_PROFILER_END(PK_PHYSICS_PRE_SEPERATION);

    LS_PROFILER_BEGIN(PK_PHYSICS_POST_SEPERATION);
    for (int i = 0; i < maxIteration; ++i)
    {
        for(auto &pair : colliderPairs_)
        {
            if (!pair.second.isTrigger)
            {
                doPostSeperation(deltaTime, pair.second);
            }
        }
    }
    LS_PROFILER_END(PK_PHYSICS_POST_SEPERATION);

    // 更新刚体运动属性
    for (auto& pair : activeBodies_)
    {
        pair.second->postUpdate(deltaTime);
    }

    // 移除不活跃的刚体
    for (auto it = activeBodies_.begin(); it != activeBodies_.end(); )
    {
        FRigidbodyPtr rigidbody = it->second;
        if (rigidbody->canSleep())
        {
            rigidbody->isActive_ = false;
            it = activeBodies_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    assert(activeBodies_.size() <= rigidbodys_.size() && "remove active rigidbody failed!");

    Profiler::getDefault()->end(PK_PHYSICS_TICK);
}

class QueryColliderPair
{
public:
    FPhysics2D *physics;
    FCollider *collider;

    bool operator()(FBVHNode *node)
    {
        if (!collider->canCollideWith(node->collider.get()))
        {
            return false;
        }

        if (physics->existColliderPair(collider, node->collider.get()))
        {
            return false;
        }

        FCollisionInfo info;
        if (collisionTest(collider, node->collider.get(), info))
        {
            physics->addColliderPair(info);
        }

        return false;
    }
};

// 查询碰撞对
void FPhysics2D::queryColliderPairs()
{
    //LS_PROFILER(PK_PHYSICS_JUDGE_PAIR);
    Profiler::getDefault()->begin(PK_PHYSICS_JUDGE_PAIR);

    QueryColliderPair query{ this, nullptr };
    for (auto& pair : activeBodies_)
    {
        SmartPtr<FRigidbody> r = pair.second;
        if (r->isStatic())
        {
            continue;
        }

        for (size_t i = 0; i < r->getNumColliders(); ++i)
        {
            LS_PROFILER(PK_PHYSICS_COLLIDERCAST);

            FCollider *collider = r->getCollider(i);
            query.collider = collider;

            dynamicTree_->queryCollider(collider->getBounds(), query);
            staticTree_->queryCollider(collider->getBounds(), query);
        }
    }

    Profiler::getDefault()->end();
}

void FPhysics2D::rebuildTree()
{
    staticTree_->rebuild();
    dynamicTree_->rebuild();
}

class QueryColliderByPoint
{
public:
    FVector2 point;
    FFloat radius;
    SmartPtr<FCollider> collider;

    QueryColliderByPoint(const FVector2 &point, FFloat radius)
    {
        this->point = point;
        this->radius = radius;
    }

    bool operator()(FBVHNode *node)
    {
        if (node->collider->overlapPoint(point, radius))
        {
            collider = node->collider;
            return true;
        }
        return false;
    }
};

FCollider* FPhysics2D::pointCast(const FVector3 & point, FFloat radius)
{
    QueryColliderByPoint query(point.toXZ(), radius);
    FBB bb(point.toXZ(), radius);
    if (!dynamicTree_->queryCollider(bb, query))
    {
        staticTree_->queryCollider(bb, query);
    }
    return query.collider.get();
}


class QueryColliderByRay
{
public:
    FRay ray;
    const FColliderFilter &filter;
    FRaycastHit &hit;
    FRaycastHit tempHit;
    bool collide = false;

    QueryColliderByRay(const FRay &_ray, const FColliderFilter &_filter, FRaycastHit &_hit)
        : ray(_ray)
        , filter(_filter)
        , hit(_hit)
    {
    }

    FFloat operator()(FBVHNode *node)
    {
        //LOG_DEBUG("test: id: %d", node->collider->getID());
        if (!node->collider->isTrigger() && filter.canCollide(node->collider->getFilter()) && node->collider->rayCast(ray, tempHit))
        {
            if (!collide || tempHit.distance < hit.distance)
            {
                hit = tempHit;
            }
            //LOG_DEBUG("raycast: id: %d, distance: %d", hit.collider->getID(), toi(hit.distance));
            collide = true;
            return tempHit.distance;
        }
        return ray.distance + FFloat(1);
    }
};

bool FPhysics2D::linecast(const FVector3 &start, const FVector3 &end, FFloat radius, const FColliderFilter &filter, FRaycastHit &hit)
{
    LS_PROFILER(PK_PHYSICS_LINECAST);

    FRay ray(start.toXZ(), end.toXZ());
    QueryColliderByRay query(ray, filter, hit);

    hit.distance = ray.distance;

    dynamicTree_->queryByRay(ray.start, ray.normal, hit.distance, query);
    staticTree_->queryByRay(ray.start, ray.normal, hit.distance, query);
    return query.collide;
}


class QueryColliderByCollider
{
    FCollider *collider;
    FCollisionInfo info;
    bool all;
public:
    std::vector<FCollider*> targets;

    QueryColliderByCollider(FCollider *_collider, bool _all)
        : collider(_collider)
        , all(_all)
    {
    }

    bool operator()(FBVHNode *node)
    {
        if (collider->canCollideWith(node->collider.get()) &&
            collisionTest(collider, node->collider.get(), info))
        {
            targets.push_back(collider != info.a ? info.a : info.b);
            return !all;
        }
        return false;
    }
};

FCollider* FPhysics2D::colliderCast(FCollider *collider)
{
    QueryColliderByCollider query(collider, false);
    if (dynamicTree_->queryCollider(collider->getBounds(), query))
    {
        return query.targets[0];
    }

    if (staticTree_->queryCollider(collider->getBounds(), query))
    {
        return query.targets[0];
    }

    return nullptr;
}

bool FPhysics2D::colliderCastAll(FCollider *collider, std::vector<FCollider*> &targets)
{
    QueryColliderByCollider query(collider, true);
    dynamicTree_->queryCollider(collider->getBounds(), query);
    staticTree_->queryCollider(collider->getBounds(), query);
    targets.swap(query.targets);
    return !targets.empty();
}

void FPhysics2D::addRigidbody(FRigidbody * rigidbody)
{
    if (rigidbody->isInPhysics())
    {
        return;
    }

    if (rigidbody->getPhysics() == nullptr)
    {
        rigidbody->setPhysics(this);
    }
    else if (rigidbody->getPhysics() != this)
    {
        LOG_ERROR("Rigidbody not created by this physics");
        return;
    }

    rigidbodys_.push_back(rigidbody);
    rigidbody->onAddToPhysicsWorld();
}

void FPhysics2D::removeRigidbody(FRigidbody * rigidbody)
{
    if (!rigidbody->isInPhysics())
    {
        return;
    }

    if (rigidbody->getPhysics() != this)
    {
        LOG_ERROR("Rigidbody not created by this physics");
        return;
    }

    if (rigidbody->isActive())
    {
        activeBodies_.erase(rigidbody->getID());
    }

    rigidbody->onRemoveFromPhysicsWorld();

    rigidbodys_.erase(
        std::remove(rigidbodys_.begin(), rigidbodys_.end(), rigidbody),
        rigidbodys_.end()
    );
}

void FPhysics2D::genCollision(Collision& collision, FCollider* collider)
{
    collision.rigidbody = collider->getRigidbody();
}

void FPhysics2D::addActiveRigidbody(FRigidbody * rigidbody)
{
    activeBodies_[rigidbody->getID()] = rigidbody;
}

void FPhysics2D::addCollider(FCollider *collider)
{
    if (collider->bInPhysics_)
    {
        return;
    }
    collider->bInPhysics_ = true;
    
    if (collider->getPhysics() == nullptr)
    {
        collider->setPhysics(this);
    }

    collider->updateTransform();

    if (collider->rigidbody_->isStatic())
    {
        staticTree_->addCollider(collider);
    }
    else
    {
        dynamicTree_->addCollider(collider);
    }
}

void FPhysics2D::removeCollider(FCollider *collider)
{
    if (!collider->bInPhysics_)
    {
        return;
    }
    
    collider->bInPhysics_ = false;
    if (collider->rigidbody_->isStatic())
    {
        staticTree_->removeCollider(collider);
    }
    else
    {
        dynamicTree_->removeCollider(collider);
    }
}

bool FPhysics2D::existColliderPair(FCollider *a, FCollider *b)
{
    if (a->getID() > b->getID())
    {
        std::swap(a, b);
    }

    uint64_t id = uint64_t(a->getID()) << 32 | uint64_t(b->getID());
    auto it = colliderPairs_.find(id);
    return it != colliderPairs_.end() && it->second.stamp == tickStamp;
}

void FPhysics2D::addColliderPair(const FCollisionInfo &info)
{
    FCollisionInfo contact = info;
    if (contact.a->getID() > contact.b->getID())
    {
        swapCollisionInfo(contact);
    }

    FCollider *a = contact.a;
    FCollider *b = contact.b;
    
    uint64_t id = uint64_t(a->getID()) << 32 | uint64_t(b->getID());

    auto it = colliderPairs_.find(id);
    if (it != colliderPairs_.end())
    {
        FColliderPair &pair = it->second;
        // 避免重复添加
        if (pair.stamp == tickStamp)
        {
            return;
        }

        pair.stamp = tickStamp;
        pair.isTrigger = a->isTrigger() || b->isTrigger() ||
            a->getRigidbody()->isKinematic() || b->getRigidbody()->isKinematic();

        FCollisionInfo &o = pair.collisionInfo;
        o.a = contact.a;
        o.b = contact.b;
        o.distance = contact.distance;
        o.normal = contact.normal;
        o.pointA = contact.pointA;
        o.pointB = contact.pointB;
    }
    else
    {
        FColliderPair pair;
        pair.id = id;
        pair.a = a;
        pair.b = b;
        pair.stamp = tickStamp;
        pair.state = FColliderPair::STATE_ENTER;
        pair.collisionInfo = contact;
        pair.isTrigger = a->isTrigger() || b->isTrigger() ||
            a->getRigidbody()->isKinematic() || b->getRigidbody()->isKinematic();
        colliderPairs_[id] = pair;
    }
}

void FPhysics2D::updateColliderPair(FFloat dt, FColliderPair &pair)
{
    SmartPtr<FCollider> a = pair.a;
    SmartPtr<FCollider> b = pair.b;

    if (pair.stamp != tickStamp)
    {
        pair.state = FColliderPair::STATE_EXIT;
    }

    if (pair.state == FColliderPair::STATE_ENTER)
    {
        LOG_VERBOSE("CollisionEnter: %d-%d", a->getID(), b->getID());
        pair.state = FColliderPair::STATE_STAY;
        if (!a->getRigidbody()->isStatic())
        {
            a->getRigidbody()->setActive(true);
        }
        if (!b->getRigidbody()->isStatic())
        {
            b->getRigidbody()->setActive(true);
        }

        /*if (a->gameObject_ != nullptr && (a->filter_.mask & b->filter_.layer) != 0)
        {
            a->gameObject_->collisionEnter(genCollision(b.get()));
        }
        if (b->gameObject_ != nullptr && (b->filter_.mask & a->filter_.layer) != 0)
        {
            b->gameObject_->collisionEnter(genCollision(a.get()));
        }*/
    }
    else if (pair.state == FColliderPair::STATE_EXIT)
    {
        LOG_VERBOSE("CollisionExit: %d-%d", a->getID(), b->getID());
        /*if (a->gameObject_ != nullptr && (a->filter_.mask & b->filter_.layer) != 0)
        {
            a->gameObject_->collisionExit(genCollision(b.get()));
        }
        if (b->gameObject_ != nullptr && (b->filter_.mask & a->filter_.layer) != 0)
        {
            b->gameObject_->collisionExit(genCollision(a.get()));
        }*/
    }

    if (pair.state == FColliderPair::STATE_STAY)
    {
        LOG_VERBOSE("CollisionStay: %d-%d, distance: %d, normal: (%d, %d)",
            a->getID(), b->getID(), toi(pair.collisionInfo.distance),
            toi(pair.collisionInfo.normal.x),
            toi(pair.collisionInfo.normal.y));

        /*if (a->gameObject_ != nullptr && (a->filter_.mask & b->filter_.layer) != 0)
        {
            a->gameObject_->collisionStay(genCollision(b.get()));
        }
        if (b->gameObject_ != nullptr && (b->filter_.mask & a->filter_.layer) != 0)
        {
            b->gameObject_->collisionStay(genCollision(a.get()));
        }*/

        // 触发器需要保持active状态，避免进入idle状态后，发生离开触发器的现象
        if (pair.isTrigger)
        {
            a->rigidbody_->setCollisionStamp(tickStamp);
            b->rigidbody_->setCollisionStamp(tickStamp);
        }
    }
}

void FPhysics2D::onColliderBBChange(FCollider * collider)
{
    if (!enableHandle_)
    {
        return;
    }

    auto tree = collider->rigidbody_->isStatic() ? staticTree_ : dynamicTree_;
    tree->updateCollider(collider);
}

uint32_t FPhysics2D::allocateID()
{
    ++idCounter;
    if (idCounter == 0)
    {
        idCounter = 1;
        LOG_ERROR("id was overflow!");
    }
    return idCounter;
}

void FPhysics2D::doPreSeperation(FFloat dt, FColliderPair & collision)
{
    FRigidbody &a = *(collision.a->getRigidbody());
    FRigidbody &b = *(collision.b->getRigidbody());

    FCollisionInfo &info = collision.collisionInfo;

    FVector3 normal = info.normal;
    FVector3 tangent(-normal.z, FFloat(0), normal.x);

    FFloat kNormal = a.getPointMoment(info.pointA, normal) + b.getPointMoment(info.pointB, normal);
    info.massNormal = FFloat(1) / kNormal;

    FFloat kTangent = a.getPointMoment(info.pointA, tangent) + b.getPointMoment(info.pointB, tangent);
    info.massTangent = FFloat(1) / kTangent;

    info.bias = biasFactor_ * FMath::max(FFloat(0), info.distance - allowedPenetration_) / dt;

    FVector3 F = normal * info.forceNormal + tangent * info.forceTangent;
    a.applyImpulse(-F);
    a.applyTorqueImpulse(info.pointA, -F);

    b.applyImpulse(F);
    b.applyTorqueImpulse(info.pointB, F);

    LOG_VERBOSE("PreSeperation: %d-%d, penetrate: %d, bias: %d, impulse(%d, %d)",
        collision.a->getID(), collision.b->getID(), toi(info.distance), toi(info.bias), toi(F.x), toi(F.y));
}

void FPhysics2D::doPostSeperation(FFloat dt, FColliderPair &collision)
{
    FRigidbody &a = *(collision.a->getRigidbody());
    FRigidbody &b = *(collision.b->getRigidbody());

    FCollisionInfo &contact = collision.collisionInfo;
    FFloat fraction = FFloat(1) - (contact.a->getFriction() + contact.a->getFriction()) / 2;

    // 计算分离力
    FVector3 relativeVelocity = a.getPointVelocity(contact.pointA) - b.getPointVelocity(contact.pointB);

    FVector3 normal = contact.normal;
    FFloat vn = relativeVelocity.dot(normal);
    FFloat dFn = (vn + contact.bias) * contact.massNormal;
    FFloat oldFn = contact.forceNormal;
    // 限制一个最大的力，避免越界
    contact.forceNormal = FMath::clamp(oldFn + dFn, FFloat(0), FFloat(1000));
    dFn = contact.forceNormal - oldFn;
    
    FVector3 F = normal * dFn;
    a.applyImpulse(-F);
    a.applyTorqueImpulse(contact.pointA, -F);

    b.applyImpulse(F);
    b.applyTorqueImpulse(contact.pointB, F);

    LOG_VERBOSE("doPostSeperation-normal: %d-%d, impulse(%d, %d), accumulate: %d, rv(%d, %d) v1(%d, %d), v2(%d, %d)",
        collision.a->getID(), collision.b->getID(), toi(F.x), toi(F.y),
        toi(contact.forceNormal),
        toi(relativeVelocity.x), toi(relativeVelocity.y),
        toi(a.velocity.x), toi(a.velocity.y),
        toi(b.velocity.x), toi(b.velocity.y));
    
    // 计算摩擦力
    relativeVelocity = a.getPointVelocity(contact.pointA) - b.getPointVelocity(contact.pointB);

    FVector3 tangent(-normal.z, FFloat(0), normal.x);
    FFloat vt = relativeVelocity.dot(tangent);
    FFloat dFt = vt * contact.massTangent;
    FFloat maxFt = fraction * contact.forceNormal;
    FFloat oldFt = contact.forceTangent;
    contact.forceTangent = FMath::clamp(oldFt + dFt, -maxFt, maxFt);
    dFt = contact.forceTangent - oldFt;

    F = tangent * dFt;
    a.applyImpulse(-F);
    a.applyTorqueImpulse(contact.pointA, -F);

    b.applyImpulse(F);
    b.applyTorqueImpulse(contact.pointB, F);

    LOG_VERBOSE("doPostSeperation-tangent: %d-%d, impulse(%d, %d), accumulate: %d, v(%d, %d)",
        collision.a->getID(), collision.b->getID(), toi(F.x), toi(F.y),
        toi(contact.forceTangent),
        toi(relativeVelocity.x), toi(relativeVelocity.y));
}

size_t FPhysics2D::getBVHNodeCount()
{
    return dynamicTree_->getNodeCount() + staticTree_->getNodeCount();
}

size_t FPhysics2D::getBVHLeafCount()
{
    return dynamicTree_->getLeafeCount() + staticTree_->getLeafeCount();
}

size_t FPhysics2D::getBVHDepth()
{
    return std::max(dynamicTree_->getDepth(), staticTree_->getDepth());
}

void FPhysics2D::debugDraw()
{
    for (SmartPtr<FRigidbody> &rigidbody : rigidbodys_)
    {
        DebugDraw::getInstance()->drawPoint(rigidbody->getBodyPosition().toXZ(), Color::red);

        for (size_t i = 0; i < rigidbody->getNumColliders(); ++i)
        {
            FCollider *c = rigidbody->getCollider(i);
            c->debugDraw();
            if (DebugDraw::getInstance()->showColliderBB)
            {
                DebugDraw::getInstance()->drawBB(c->getBounds(), Color::red);
            }
        }
    }

    if (DebugDraw::getInstance()->showBVHTree)
    {
        staticTree_->debugDraw();
        dynamicTree_->debugDraw();
    }
}

size_t FPhysics2D::getMemorySize()
{
    return sizeof(*this) +
        dynamicTree_->getMemorySize() +
        staticTree_->getMemorySize() +
        gjk_->getMemorySize() +
        rigidbodys_.capacity() * sizeof(FRigidbodyPtr) +
        activeBodies_.size() * sizeof(std::map<uint32_t, FRigidbodyPtr>::value_type) +
        colliderPairs_.size() * sizeof(std::map<uint64_t, FColliderPair>::value_type) +
        staticRigidbody_->getMemorySize();
}

void FPhysics2D::setBVHEdgeCoef(FFloat coef)
{
    dynamicTree_->setEdgeCoef(coef);
    staticTree_->setEdgeCoef(coef);
}

FFloat FPhysics2D::getBVHEdgeCoef() const
{
    return dynamicTree_->getEdgeCoef();
}

void FPhysics2D::setStaticShapeFilter(uint32_t group, uint32_t layer, uint32_t mask)
{
    staticShapeFilter_.set(group, layer, mask);
}

NS_FXP_END
