//////////////////////////////////////////////////////////////////////
/// Desc  DebugDraw
/// Time  2020/12/09
/// Autor youlanhai
//////////////////////////////////////////////////////////////////////

#include "DebugDraw.hpp"

NS_FXP_BEGIN

DebugDraw *DebugDraw::s_instance = nullptr;

DebugDraw* DebugDraw::getInstance()
{
    if (nullptr == s_instance)
    {
        s_instance = new DebugDraw();
    }
    return s_instance;
}

DebugDraw::DebugDraw()
    : lineWidth(4)
    , circleSplitAngle(10)
{

}

DebugDraw::~DebugDraw()
{

}

void DebugDraw::drawPoint(const FVector2 &point, const Color &color)
{
    FFloat delta = lineWidth / 2;

    addVertex(point.x - delta, point.y - delta, color); // left bottom
    addVertex(point.x - delta, point.y + delta, color); // left top
    addVertex(point.x + delta, point.y + delta, color); // right top

    addVertex(point.x + delta, point.y + delta, color); // right top
    addVertex(point.x + delta, point.y - delta, color); // right bottom
    addVertex(point.x - delta, point.y - delta, color); // left bottom
}

void DebugDraw::drawLine(const FVector2 &start, const FVector2 &end, const Color &color)
{
    FVector2 normal = end - start;
    if (normal.isZero())
    {
        return;
    }

    normal.normalize();
    FVector2 tangent(-normal.y, normal.x);

    FFloat halfLineWidth = lineWidth / 2;
    FVector2 delta = tangent * halfLineWidth;

    addVertex(start - delta, color); // left bottom
    addVertex(start + delta, color); // left top
    addVertex(end   + delta, color); // right top

    addVertex(end   + delta, color); // right top
    addVertex(end   - delta, color); // right bottom
    addVertex(start - delta, color); // left bottom
}


void DebugDraw::drawCircle(const FVector2 &center, FFloat radius, const Color &color)
{
    int n = 360 / circleSplitAngle;

    FVector2 lastPos = center + FVector2(radius, FFloat(0));

    for (int i = 1; i <= n; ++i)
    {
        FFloat angle(i * circleSplitAngle);
        FVector2 normal(FMath::cos(angle), FMath::sin(angle));
        FVector2 pos = center + normal * radius;
        
        drawLine(lastPos, pos, color);

        lastPos = pos;
    }
}

void DebugDraw::drawRect(const FVector2 &min, const FVector2 &max, const Color &color)
{
    drawLine(FVector2(min.x, min.y), FVector2(min.x, max.y), color); // left
    drawLine(FVector2(max.x, min.y), FVector2(max.x, max.y), color); // right

    drawLine(FVector2(min.x, max.y), FVector2(max.x, max.y), color); // top
    drawLine(FVector2(min.x, min.y), FVector2(max.x, min.y), color); // bottom
}

void DebugDraw::drawPolygon(const FVector2 *points, size_t count, const Color &color)
{
    for (size_t i = 1; i <= count; ++i)
    {
        drawLine(points[i - 1], points[i % count], color);
    }
}


void DebugDraw::drawSolidCircle(const FVector2 &center, FFloat radius, const Color &color)
{
    int n = 360 / circleSplitAngle;

    FVector2 lastPos = center + FVector2(radius, FFloat(0));

    for (int i = 1; i <= n; ++i)
    {
        FFloat angle(i * circleSplitAngle);
        FVector2 normal(FMath::cos(angle), FMath::sin(angle));
        FVector2 pos = center + normal * radius;
        
        addVertex(center, color);
        addVertex(lastPos, color);
        addVertex(pos, color);

        lastPos = pos;
    }
}

void DebugDraw::drawSolidRect(const FVector2 &min, const FVector2 &max, const Color &color)
{
    addVertex(min.x, min.y, color); // left bottom
    addVertex(min.x, max.y, color); // left top
    addVertex(max.x, max.y, color); // right top

    addVertex(max.x, max.y, color); // right top
    addVertex(max.x, min.y, color); // right bottom
    addVertex(min.x, min.y, color); // left bottom
}

void DebugDraw::drawSolidPolygon(const std::vector<FVector2> &points, const Color &color)
{
    for (size_t i = 2; i < points.size(); ++i)
    {
        addVertex(points[0], color);
        addVertex(points[i - 1], color);
        addVertex(points[i], color);
    }
}

NS_FXP_END
