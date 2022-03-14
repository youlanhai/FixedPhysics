//////////////////////////////////////////////////////////////////////
/// Desc  DebugDraw
/// Time  2020/12/09
/// Autor youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "Color.hpp"
#include "math/FVector2.hpp"
#include "physics2d/FBB.hpp"
#include <vector>

NS_FXP_BEGIN

struct VertexXYZColor
{
    float x, y, z;
    Color color;
};

class FXP_API DebugDraw
{
    static DebugDraw *s_instance;
    DebugDraw();
    ~DebugDraw();
public:
    static DebugDraw* getInstance();

    std::vector<VertexXYZColor> vertices;
    FFloat lineWidth;
    int circleSplitAngle;

    bool showColliderBB = false;

    bool showBVHTree = true;
    bool showBVHLeaf = true;
    bool showBVHNode = true;
    int  showBVHDepth = -1;

    void clear()
    {
        vertices.clear();
    }

    void drawPoint(const FVector2 &point, const Color &color);

    void drawLine(const FVector2 &start, const FVector2 &end, const Color &color);

    void drawCircle(const FVector2 &center, FFloat radius, const Color &color);

    void drawRect(const FVector2 &min, const FVector2 &max, const Color &color);

    void drawBB(const FBB &bb, const Color &color) { drawRect(bb.min, bb.max, color); }

    void drawPolygon(const FVector2 *points, size_t count, const Color &color);

    void drawSolidCircle(const FVector2 &center, FFloat radius, const Color &color);

    void drawSolidRect(const FVector2 &min, const FVector2 &max, const Color &color);

    void drawSolidBB(const FBB &bb, const Color &color) { drawSolidRect(bb.min, bb.max, color); }

    void drawSolidPolygon(const std::vector<FVector2> &points, const Color &color);

    void addVertex(const VertexXYZColor &v)
    {
        vertices.push_back(v);
    }

    void addVertex(FFloat x, FFloat y, const Color &color)
    {
        vertices.push_back(VertexXYZColor{float(x), float(y), 0, color});
    }

    void addVertex(const FVector2 &position, const Color &color)
    {
        vertices.push_back(VertexXYZColor{float(position.x), float(position.y), 0, color});
    }
};

NS_FXP_END
