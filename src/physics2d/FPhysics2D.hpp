//////////////////////////////////////////////////////////////////////
/// Desc  Physics2D
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once

#include "common/SmartPtr.hpp"
#include "common/IRefCount.hpp"
#include "math/FVector2.hpp"
#include "math/FVector3.hpp"
#include "FRay.hpp"
#include "FPhysicsDef.hpp"

#include <vector>
#include <map>

NS_FXP_BEGIN

class FBVHTree;
class FGJK;

/** 基于定点数的2D物理引擎 */
class FXP_API FPhysics2D : public IRefCount
{
public:
    FPhysics2D();
    ~FPhysics2D();

    /** 初始化 */
    virtual void init();

    /** 清空所有物理数据。 */
    void clear();

    void tick(FFloat deltaTime);

    void addRigidbody(FRigidbody *rigidbody);

    void removeRigidbody(FRigidbody *rigidbody);

    virtual void genCollision(Collision &collision, FCollider* collider);

public:

    /** 查询与点相交的碰撞体 */
    FCollider* pointCast(const FVector3 &point, FFloat radius);
    
    /** 射线拾取。查询与射线相交且距离最近的碰撞体 */
    bool linecast(const FVector3 &start, const FVector3 &end, FFloat radius, const FColliderFilter &filter, FRaycastHit &hit);
    //bool linecastAll(const FRay &ray, std::vector<FRaycastHit> &hits);

    /** 查询与collider相交的碰撞体 */
    FCollider* colliderCast(FCollider *collider);

    /** 查询与collider相交的所有碰撞体 */
    bool colliderCastAll(FCollider *collider, std::vector<FCollider*> &targets);

    FRigidbody* getStaticRigidbody(){ return staticRigidbody_.get(); }

    /** 获取结点总数量，包括叶结点 */
    size_t getBVHNodeCount();
    /** 获取叶结点数量。也就是collider的数量 */
    size_t getBVHLeafCount();
    /** 返回树的最大深度 */
    size_t getBVHDepth();

    size_t getRigidbodyCount() { return rigidbodys_.size(); }
    size_t getActiveRigidbodyCount() { return activeBodies_.size(); }
    size_t getCollisionPairCount() { return colliderPairs_.size(); }
    
    const FVector3& getGravity() const { return gravity_; }
    void setGravity(const FVector3 &gravity) { gravity_ = gravity; }
    
    FFloat getDamping() const { return damping_; }
    /** 设置速度衰减。每帧速度衰减的比例 */
    void setDamping(FFloat damping){ damping_ = damping; }
    
    /** 设置速度idle阀值。速度持续低于此值会进入休眠期 */
    void setIdleSpeedThreshold(FFloat threshold){ sleepSpeedThreshold = threshold; }
    /** 获取速度idle阀值 */
    FFloat getIdleSpeedThreshold() const { return sleepSpeedThreshold; }

    /** 设置进入休眠期的时间阈值 */
    void setSleepTimeThreshold(FFloat threshold) { sleepTimeThreshold = threshold; }
    FFloat getSleepTimeThreshold() { return sleepTimeThreshold; }

    /// 获取计算迭代次数
    int getSolverIterations() const { return maxIteration; }
    /// 设置计算迭代次数
    void setSolverIterations(int v) { maxIteration = v; }

    /// 获取穿透容差
    FFloat getCollisionSlop() const {return allowedPenetration_; }
    /// 设置穿透容差
    void setCollisionSlop(FFloat v) { allowedPenetration_ = v; }
    
    /// 获取分开速率
    FFloat getCollisionBias() const { return biasFactor_; }
    /// 设置分离速率，默认0.1
    void setCollisionBias(FFloat v) { biasFactor_ = v; }
    
    size_t getMemorySize();

    int getTickStamp() const { return tickStamp; }

    /** 停止处理碰撞回调 */
    void stopHandleEvents() { enableHandle_ = false; };

    /** 设置BVH松散度。碰撞体包围盒外扩的比例 */
    void setBVHEdgeCoef(FFloat coef);
    FFloat getBVHEdgeCoef() const;

public:
    /// 获取世界统一的y坐标值
    void setWorldY(FFloat y){ worldY_ = y; }
    /// 设置世界的y坐标值
    FFloat getWorldY() const { return worldY_; }
    
    /// 设置是否使用统一的世界y值
    void setWorldYEnable(bool enable){ worldYEnabled_ = enable; }
    
    /// 是否使用统一的世界y值
    bool isWorldYEnabled() const { return worldYEnabled_; }
    
    /** 设置静态shape的碰撞过滤参数。仅在加载物理数据之前设置有效 */
    void setStaticShapeFilter(uint32_t group, uint32_t layer, uint32_t mask);

    /** 设置自动重建bvh的阈值 */
    void setRebuildTreeThreshold(int threshold) { rebuildTreeThreshold_ = threshold; }

    /** 手动重建bvh */
    void rebuildTree();
    
public: // 内部方法，不会导出给lua。

    /** @private */
    uint32_t allocateID();

    /** @private */
    void debugDraw();
    
    /** @private 碰撞体包围盒发生了变化 */
    void onColliderBBChange(FCollider *collider);

    /** @private 是否已经存在碰撞对了 */
    bool existColliderPair(FCollider *a, FCollider *b);
    
    /** @private 添加碰撞对 */
    void addColliderPair(const FCollisionInfo &info);
    
    /** @private */
    FGJK* getGJK() { return gjk_; }

    /** @private */
    const std::map<uint64_t, FColliderPair>& getColliderPairs() const { return colliderPairs_; }
    
private:
    friend class FRigidbody;
    
    void addActiveRigidbody(FRigidbody *rigidbody);

    void addCollider(FCollider *collider);
    void removeCollider(FCollider *collider);
    
    void queryColliderPairs();
    void updateColliderPair(FFloat dt, FColliderPair &pair);
    
    void doPreSeperation(FFloat dt, FColliderPair &collision);
    void doPostSeperation(FFloat dt, FColliderPair &collision);
    
private:
    std::vector<FRigidbodyPtr> rigidbodys_;
    std::map<uint32_t, FRigidbodyPtr> activeBodies_;
    std::map<uint64_t, FColliderPair> colliderPairs_;

    FBVHTree*       dynamicTree_;
    FBVHTree*       staticTree_;
    FGJK*           gjk_;
    FRigidbodyPtr   staticRigidbody_;
    int             tickStamp = 0;
    int             maxIteration = 5;

    /** 重新构建aabb树的阈值 */
    int             rebuildTreeThreshold_ = 100;

    /** 速度衰减系数 */
    FFloat          damping_ = FFloat(0, 9, 7);
    /** 重力加速度 */
    FVector3        gravity_;

    /** 进入休眠状态的移动速度阈值。要跟damping_结合起来设置，避免精度问题导致无法衰减到休眠速度。*/
    FFloat          sleepSpeedThreshold = FFloat(0, 0, 5);
    /** 进入休眠状态的时间阈值 */
    FFloat          sleepTimeThreshold = FFloat(0, 5);

    FFloat          biasFactor_ = FFloat(0, 1);
    FFloat          allowedPenetration_ = FFloat(0, 0, 1);

    uint32_t        idCounter = 0;
    
    /// 世界的y坐标。由于是2d物理引擎，y只能取一个平面
    FFloat         worldY_ = FFloat(0);
    
    /// 是否使用统一的世界y坐标。
    bool            worldYEnabled_ = false;
    bool            enableHandle_ = true;

    /// 静态shape的碰撞参数
    FColliderFilter   staticShapeFilter_;
};

NS_FXP_END
