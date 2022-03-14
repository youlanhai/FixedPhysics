//////////////////////////////////////////////////////////////////////
/// Desc  FCollider
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "FCollider.hpp"
#include "FRigidbody.hpp"
#include "debug/DebugDraw.hpp"
#include "FPhysics2D.hpp"
#include "FGJK.hpp"
#include "math/FMath.hpp"

NS_FXP_BEGIN

static inline Color getColor(FCollider *collider)
{
    return collider->getRigidbody()->isActive() ? Color::red : Color::green;
}

size_t FCollider::getMemorySize()
{
    return sizeof(*this);
}

void FCollider::setPhysics(FPhysics2D *physics)
{
    if (physics == physics_)
    {
        return;
    }
    
    physics_ = physics;
    
    if (physics_ != nullptr)
    {
        id_ = physics_->allocateID();
    }
}

bool FCollider::canCollideWith(FCollider *collider)
{
    return this != collider &&
        rigidbody_ != collider->rigidbody_ &&
        filter_.canCollide(collider->filter_);
}

FCircleCollider::FCircleCollider()
: radius_(FFloat(1))
{
    type_ = FT_CIRCLE;
}

FCircleCollider::FCircleCollider(FFloat radius)
: radius_(radius)
{
    type_ = FT_CIRCLE;
}

FCircleCollider::FCircleCollider(FFloat radius, const FVector3 & center)
: center_(center)
, radius_(radius)
{
    type_ = FT_CIRCLE;
}


void FCircleCollider::updateTransform()
{
    tCenter_ = rigidbody_->getMatrix().transformPoint(FVector2(center_.x, center_.z));
    FVector2 tCorner = rigidbody_->getMatrix().transformPoint(FVector2(center_.x + radius_, center_.z));

    tRadius_ = tCenter_.distanceTo(tCorner);

    FVector2 size = FVector2(tRadius_, tRadius_);
    bb_.min = tCenter_ - size;
    bb_.max = tCenter_ + size;
}

void FCircleCollider::debugDraw()
{
    DebugDraw::getInstance()->drawCircle(tCenter_, tRadius_, getColor(this));
}

FVector2 FCircleCollider::getFirstVertex()
{
    return tCenter_ + FVector2(tRadius_, FFloat(0));
}

FVector2 FCircleCollider::getFarthestPointInDirection(const FVector2 & dir)
{
    return tCenter_ + dir * (tRadius_ / dir.length());
}

bool FCircleCollider::overlapPoint(const FVector2 & point, FFloat radius)
{
    radius += tRadius_;
    return point.distanceToSq(tCenter_) <= radius * radius;
}

bool FCircleCollider::rayCast(const FRay &ray, FRaycastHit &hit)
{
    // 算法参考: 《3D数学基础-图形与游戏开发》 13.12

    FVector2 e = tCenter_ - ray.start;
    
    // 起点在圆内
    FFloat eLengthSq = e.lengthSq();
    if (eLengthSq <= tRadius_ * tRadius_)
    {
        hit.distance = 0;
        hit.normal.setXZ(ray.normal);
        hit.point.setXZ(ray.start);
        hit.collider = this;
        return true;
    }
    
    FFloat a = e.dot(ray.normal);

    FFloat delta = tRadius_ * tRadius_ - eLengthSq + a * a;
    // 不相交
    if (delta < 0)
    {
        return false;
    }
    
    FFloat t = a - FMath::sqrt(delta);
    if (t < FFloat(0) || t > FFloat(ray.distance))
    {
        return false;
    }

    hit.distance = t;
    hit.normal.setXZ(ray.normal);
    hit.point.setXZ(ray.start + ray.normal * hit.distance);
    hit.collider = this;
    return true;
}

//-------------------------
// FSegmentCollider
//-------------------------

FSegmentCollider::FSegmentCollider()
{
    type_ = FT_SEGMENT;
}

FSegmentCollider::FSegmentCollider(const FVector3 &a, const FVector3 &b, FFloat radius)
{
    type_ = FT_SEGMENT;
    setEndpoints(a, b);
}

void FSegmentCollider::setEndpoints(const FVector3& a, const FVector3 &b)
{
    start = a;
    end = b;
    normal = b - a;
    normal.normalize();
}

void FSegmentCollider::updateTransform()
{
    tStart = rigidbody_->getMatrix().transformPoint(start.toXZ());
    tEnd = rigidbody_->getMatrix().transformPoint(end.toXZ());

    tNormal = tEnd - tStart;
    tNormal.normalize();

    bb_.resetWithPoint(tStart, tEnd);
    bb_.normalize(); // 避免维度为0
}

void FSegmentCollider::debugDraw()
{
    DebugDraw::getInstance()->drawLine(tStart, tEnd, getColor(this));
}

FVector2 FSegmentCollider::getFirstVertex()
{
    return tStart;
}

FVector2 FSegmentCollider::getFarthestPointInDirection(const FVector2 & dir)
{
    if (tStart.dot(dir) > tEnd.dot(dir))
    {
        return tStart;
    }
    else
    {
        return tEnd;
    }
}

bool FSegmentCollider::overlapPoint(const FVector2 & point, FFloat radius)
{
    FVector2 ab = tEnd - tStart;
    FVector2 ap = point - tStart;
    
    if (ab.isZero())
    {
        return point.distanceToSq(ab) <= radius * radius;
    }

    // 计算point到线段最近点的距离，是否小于radius
    FFloat projection = ap.dot(ab) / ab.lengthSq();
    projection = FMath::clamp01(projection);
    FVector2 crossPt = tStart + ab * projection;
    return point.distanceToSq(crossPt) < radius * radius;
}

bool FSegmentCollider::rayCast(const FRay &ray, FRaycastHit &hit)
{
    const FVector2& a = ray.end - ray.start;
    const FVector2& b = tEnd - tStart;
    FVector2 c = tStart - ray.start;
    
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
    
    hit.distance = ray.distance * t1;
    hit.normal.setXZ(ray.normal);
    hit.point.setXZ(ray.start + ray.normal * hit.distance);
    hit.collider = this;
    return true;
}

//-------------------------
// FPolygonCollider
//-------------------------

FPolygonCollider::FPolygonCollider()
{
    type_ = FT_POLYGON;
}

FPolygonCollider::FPolygonCollider(size_t count, const FVector3 *verts, bool convex)
{
    type_ = FT_POLYGON;
    setVertices(verts, count, convex);
}

FPolygonCollider::FPolygonCollider(FFloat width, FFloat heigh)
{
    type_ = FT_POLYGON;

    FFloat dx = width / 2;
    FFloat dy = heigh / 2;
    
    FVector3 verts[] = {
        {-dx, FFloat(0), -dy},
        {-dx, FFloat(0), dy},
        {dx, FFloat(0), dy},
        {dx, FFloat(0), -dy},
    };
    setVertices(verts, 4, true);
}

FPolygonCollider::FPolygonCollider(const FBB &bb)
{
    type_ = FT_POLYGON;
    FVector3 verts[] = {
        {bb.min.x, FFloat(0), bb.min.y},
        {bb.min.x, FFloat(0), bb.max.y},
        {bb.max.x, FFloat(0), bb.max.y},
        {bb.max.x, FFloat(0), bb.min.y},
    };
    setVertices(verts, 4, true);
}

void FPolygonCollider::updateTransform()
{
    bb_.reset();
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        FVector2 point = rigidbody_->getMatrix().transformPoint(vertices[i].toXZ());
        tVertices[i] = point;
        bb_.add(point);
    }
}

void FPolygonCollider::debugDraw()
{
    DebugDraw::getInstance()->drawPolygon(tVertices.data(), tVertices.size(), getColor(this));
}

void FPolygonCollider::setVertices(const FVector2 * pVertices, size_t count, bool convex)
{
    vertices.resize(count);
    tVertices.resize(count);

    for (size_t i = 0; i < count; ++i)
    {
        vertices[i].setXZ(pVertices[i]);
    }
    
    if (!convex)
    {
        convertToConvex();
    }
}

void FPolygonCollider::setVertices(const FVector3 * pVertices, size_t count, bool convex)
{
    vertices.resize(count);
    tVertices.resize(count);

    for (size_t i = 0; i < count; ++i)
    {
        vertices[i] = pVertices[i];
    }
    
    if (!convex)
    {
        convertToConvex();
    }
}

FVector2 FPolygonCollider::getFirstVertex()
{
    return tVertices[0];
}

FVector2 FPolygonCollider::getFarthestPointInDirection(const FVector2 & dir)
{
    FFloat maxDistance = FMath::FloatMin;
    size_t maxIndex = 0;
    for (size_t i = 0; i < tVertices.size(); ++i)
    {
        FFloat distance = tVertices[i].dot(dir);
        if (distance > maxDistance)
        {
            maxDistance = distance;
            maxIndex = i;
        }
    }
    return tVertices[maxIndex];
}

bool FPolygonCollider::overlapPoint(const FVector2 & point, FFloat radius)
{
    return containsPoint(tVertices.data(), tVertices.size(), point);
}

bool FPolygonCollider::rayCast(const FRay &ray, FRaycastHit &hit)
{
    const FVector2& a = ray.end - ray.start;
    FFloat tMin = FMath::FloatMax;
    bool bIntersect = false;
    
    for (size_t i = 0; i < tVertices.size(); ++i)
    {
        const FVector2& A = tVertices[i];
        const FVector2& B = tVertices[(i + 1) % tVertices.size()];
        
        FVector2 b = B - A;
//        b.normalize();
        FVector2 c = A - ray.start;
        
        FFloat denominator = a.x * b.y - a.y * b.x;
        if (denominator == 0)
        {
            continue;
        }
        
        FFloat t1 = (c.x * b.y - c.y * b.x) / denominator;
        FFloat t2 = (c.x * a.y - c.y * a.x) / denominator;
        if (t1 < FFloat(0) || t1 > FFloat(1) ||
            t2 < FFloat(0) || t2 > FFloat(1))
        {
            continue;
        }
        
        bIntersect = true;
        if (t1 < tMin)
        {
            tMin = t1;
        }
    }
    
    if (!bIntersect)
    {
        return false;
    }
    
    hit.distance = ray.distance * tMin;
    hit.normal.setXZ(ray.normal);
    hit.point.setXZ(ray.start + ray.normal * hit.distance);
    hit.collider = this;
    return true;
}

void FPolygonCollider::convertToConvex()
{
    
    // TODO 转换成凸多边形
    
}
    
size_t FPolygonCollider::getMemorySize()
{
    return FCollider::getMemorySize() +
        vertices.capacity() * sizeof(FVector3) +
        tVertices.capacity() * sizeof(FVector2);
}

NS_FXP_END
