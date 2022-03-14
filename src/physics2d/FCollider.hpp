//////////////////////////////////////////////////////////////////////
/// Desc  FCollider
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "common/IRefCount.hpp"
#include "math/FVector2.hpp"
#include "math/FVector3.hpp"
#include "FBB.hpp"
#include "FPhysicsDef.hpp"

#include <vector>

NS_FXP_BEGIN

class FRay;

class FXP_API FCollider : public IRefCount
{
public:
    FCollider() = default;

    virtual void updateTransform()
    {}

    virtual void debugDraw()
    {}

    virtual size_t getMemorySize();

    virtual FVector2 getFirstVertex() = 0;
    virtual FVector2 getFarthestPointInDirection(const FVector2 &dir) = 0;
    virtual bool overlapPoint(const FVector2 &point, FFloat radius) = 0;
    virtual bool rayCast(const FRay &ray, FRaycastHit &hit) = 0;
    
    uint32_t getID() { return id_; }
    FColliderType getType() const { return type_; }
    
    /// 获取当前所属的刚体
    FRigidbody* getRigidbody(){ return rigidbody_; }
    
    /// 获取当前所属的物理对象。如果没有加到世界中，则返回NULL
    FPhysics2D* getPhysics(){ return physics_; }
    /// 内部方法
    void setPhysics(FPhysics2D *physics);

    /** 是否已加入到物理世界中 */
    bool isInPhysics() const { return bInPhysics_; }

    /// 获取2D包围盒。@see BoundingBox
    inline const FBB& getBounds(){ return bb_; }
    
    /// 设置为触发器
    inline void setTrigger(bool trigger){ isTrigger_ = trigger; }
    /// 是否是触发器
    inline bool isTrigger(){ return isTrigger_; }
    
    /// 设置弹力。物体间反弹的力
    inline void setElasticity(FFloat v){ elasticity_ = v; }
    /// 获取弹力
    inline FFloat getElasticity(){ return elasticity_; }
    
    /// 设置摩擦力
    inline void setFriction(FFloat v){ fraction_ = v; }
    /// 获取摩擦力
    inline FFloat getFriction(){ return fraction_; }
    
    /** @brief 设置碰撞过滤器，用于判断Collider可以和哪种Collider发生碰撞。
     *  @param filter   碰撞过滤器。@see FColliderFilter, getFilter
     */
    inline void setFilter(const FColliderFilter &filter){ filter_ = filter; }
    /// 获取碰撞过滤器。@see ColliderFilter, setFilter
    inline const FColliderFilter& getFilter(){ return filter_; }
    
    /// 设置碰撞过滤器的组属性。同组物体不发生碰撞。@see setFilter
    inline void setGroup(uint32_t group) { filter_.group = group; }
    /// 设置碰撞过滤器的组属性。@see setFilter, setGroup
    inline uint32_t getGroup(){ return filter_.group; }
    
    /// 设置碰撞过滤器的层属性。如果自己的layer & 别人的mask 不为0，则会发生碰撞。@see setFilter
    inline void setLayer(uint32_t m) { filter_.layer = m; }
    /// 获取碰撞过滤器的层属性。@see setFilter, setLayer
    inline uint32_t getLayer(){ return filter_.layer; }
    
    /// 设置碰撞过滤器的掩码属性。如果自己的layer & 别人的mask 不为0，则会发生碰撞。@see setFilter
    inline void setMask(uint32_t m) { filter_.mask = m; }
    /// 获取碰撞过滤器的掩码属性。@see setFilter, setMask
    inline uint32_t getMask(){ return filter_.mask; }

    bool canCollideWith(FCollider *other);

    void setUserData(void* userData) { userData_ = userData; }
    void* getUserData() { return userData_; }
    
protected:
    FRigidbody*     rigidbody_ = nullptr;
    FPhysics2D*     physics_ = nullptr;
    void*           userData_ = nullptr;
    uint32_t        id_ = 0;
    FColliderType   type_ = FT_CIRCLE;
    FFloat          fraction_ = 0;
    FFloat          elasticity_ = 0;
    
    FColliderFilter filter_;

    /** 坐标变换之后的包围盒 */
    FBB             bb_;
    bool            isTrigger_ = false;
    bool            bInPhysics_ = false;
    
    friend class FRigidbody;
    friend class FPhysics2D;
};

class FXP_API FCircleCollider : public FCollider
{
public:
    FCircleCollider();
    FCircleCollider(FFloat radius);
    FCircleCollider(FFloat radius, const FVector3 & center);
    
    inline void setRadius(FFloat radius){ radius_ = radius; }
    inline FFloat getRadius(){ return radius_; }
    
    inline void setOffset(const FVector3 &offset){ center_ = offset; }
    inline const FVector3& getOffset(){ return center_; }
    
public: // 内部方法
    
    void updateTransform() override;
    void debugDraw() override;

    virtual FVector2 getFirstVertex() override;
    virtual FVector2 getFarthestPointInDirection(const FVector2 &dir) override;
    virtual bool overlapPoint(const FVector2 &point, FFloat radius) override;
    virtual bool rayCast(const FRay &ray, FRaycastHit &hit) override;
    
    const FVector2& getWorldCenter() const { return tCenter_; }
    FFloat getWorldRadius() const { return tRadius_; }
    
private:
    
    FVector3 center_;
    FFloat radius_;
    
    /** 变换之后的坐标 */
    FVector2 tCenter_;
    FFloat tRadius_;
};

class FXP_API FSegmentCollider : public FCollider
{
public:
    FSegmentCollider();
    
    /** @brief 构造线段碰撞体。
     *  @param a            线段起点
     *  @param b            线段终点
     *  @param radius      圆角半径。用于把边角处理成圆角
     */
    FSegmentCollider(const FVector3 &a, const FVector3 &b, FFloat radius = FFloat(0));
    
    /// 设置终点。线段组的两端
    void setEndpoints(const FVector3& a, const FVector3 &b);
    
    /// 获取起点
    const FVector3& getPointA(){ return start; }
    
    /// 获取终点
    const FVector3& getPointB(){ return end; }
    
    /// 获取方向
    const FVector3& getNormal(){ return normal; }
    
    void updateTransform() override;
    void debugDraw() override;

    virtual FVector2 getFirstVertex() override;
    virtual FVector2 getFarthestPointInDirection(const FVector2 &dir) override;
    virtual bool overlapPoint(const FVector2 &point, FFloat radius) override;
    virtual bool rayCast(const FRay &ray, FRaycastHit &hit) override;
    
    const FVector2& getWorldStart() const { return tStart; }
    const FVector2& getWorldEnd() const { return tEnd; }
    const FVector2& getWorldNormal() const { return tNormal; }
    
private:
    
    FVector3 start, end, normal;
    FVector2 tStart, tEnd, tNormal;
};

class FXP_API FPolygonCollider : public FCollider
{
public:
    FPolygonCollider();
    
    /// 生成多边形collider，传递进来的数据需要是多边形
    FPolygonCollider(size_t count, const FVector3 *verts, bool convex = true);
    /// 生成矩形collider
    FPolygonCollider(FFloat width, FFloat heigh);
    /// 生成矩形collider
    FPolygonCollider(const FBB &bb);
    
    /// 获取顶点数量
    size_t getCount(){ return vertices.size(); }
    
    /// 获取顶点坐标
    const FVector3& getVertex(size_t index){ return vertices[index]; }
    
    /** 设置顶点数据
     *  @param pVertices    顶点坐标数组
     *  @param count        顶点数量
     *  @param convex       是否是凸多边形。如果不是，会自动转换。
    */
    void setVertices(const FVector2 *pVertices, size_t count, bool convex = true);
    void setVertices(const FVector3 *pVertices, size_t count, bool convex = true);
    
    virtual size_t getMemorySize() override;
    
public: // 内部方法
    
    void updateTransform() override;
    void debugDraw() override;

    const FVector2* getWorldVertices() { return tVertices.data(); }

    virtual FVector2 getFirstVertex() override;
    virtual FVector2 getFarthestPointInDirection(const FVector2 &dir) override;
    virtual bool overlapPoint(const FVector2 &point, FFloat radius) override;
    virtual bool rayCast(const FRay &ray, FRaycastHit &hit) override;
    
private:
    void convertToConvex();
    
    /** 原始顶点数据  */
    std::vector<FVector3> vertices;
    
    /** 变换后的顶点数组 */
    std::vector<FVector2> tVertices;
};

NS_FXP_END
