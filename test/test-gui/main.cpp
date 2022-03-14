//////////////////////////////////////////////////////////////////////
/// Desc  main
/// Time  2020/12/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////
#include <cstdio>
#include "Application.hpp"

#include <algorithm>

#include "physics2d/FPhysicsAPI.hpp"
#include "debug/DebugDraw.hpp"

#include "DebugRenderer.hpp"
#include "Matrix.hpp"
#include "Camera.hpp"

#include "common/SmartPtr.hpp"
#include "debug/LogTool.hpp"
#include "debug/TestTool.hpp"
#include "debug/Profiler.hpp"

NS_FXP_USING;

static const char *USAGE = "usage:\n"
"TestPhysics.exe [physicsFilePath]\n"
"space      pause/play\n"
"1          switch AABB show level\n"
"MouseLeft:   select and drag\n"
"MouseRight   apply force to selection\n"
"MosueWheel   zoom camera\n"
"W/S/A/D/Q/E  control camera\n"
;

static const float PI = 3.1415926f;

class MainApp : public Application
{
public:

    Camera camera;
    SmartPtr<FPhysics2D> physics;

    std::vector<FRigidbody*> rigidbodies;
    FFloat physicsDeltaTime;
    SmartPtr<FCollider> selectedCollider;
    Vector3f selectedWorldPos;
    FVector3 selectedRigidbodyPos;
    
    FRay rayCast;

    /* 是否自动执行物理自动tick:
    空格键: 暂停/恢复
    鼠标左键: 暂停的情况下，单步骤执行
    */
    bool autoTick = false;

    float fov;

    Vector2f lastDragPos;
    Vector3f lastCameraPos;

    std::string physicsFilePath;

    bool onCreate() override
    {
        DebugRenderer::Init();

        lockFrameTime_ = 1 / 60.0f;
        DebugDraw::getInstance()->lineWidth = FFloat(0.1f);

        physicsDeltaTime = FFloat(1.f / 60.f);
        fov = 50 * PI / 180.f;

        glDisable(GL_CULL_FACE);
        camera.lookAt(Vector3f(0, 0, -20), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
        camera.setMoveSpeed(10.0f);

        createPhysicsWorld();
        return true;
    }

    void onDestroy() override
    {
        for (auto rigidbody : rigidbodies)
        {
            rigidbody->release();
        }

        physics->clear();
        physics = nullptr;
    }

    void onSizeChange(int width, int height) override
    {
        camera.setPerspective(fov, float(width) / height, 1, 10000);
    }

    void onKey(int key, int scancode, int action, int mods) override
    {
        Application::onKey(key, scancode, action, mods);

        if (action == GLFW_RELEASE)
        {
            switch (key)
            {
            case GLFW_KEY_SPACE:
                autoTick = !autoTick;
                break;
            case GLFW_KEY_1:
                DebugDraw::getInstance()->showBVHDepth++;
                if (DebugDraw::getInstance()->showBVHDepth > physics->getBVHDepth())
                {
                    DebugDraw::getInstance()->showBVHDepth = -1;
                }
                break;
            default:
                break;
            }
        }
    }

    Vector3f screenPosToWorld(const Vector2f &screenPos)
    {
        float z = 0 - camera.getPosition().z;
        return camera.screenPosToWorld(Vector3f(getCursorPos(), z));
    }

    void onMouseButton(int button, int action, int mods) override
    {
        if (button == GLFW_MOUSE_BUTTON_1)
        {
            if (action == GLFW_PRESS)
            {
                Vector2f mosuePos = getCursorPos();
                Vector3f worldPos = screenPosToWorld(mosuePos);
                selectedWorldPos = worldPos;
                
                FVector3 fPos(FFloat(worldPos.x), FFloat(0), FFloat(worldPos.y));

                selectedCollider = physics->pointCast(fPos, FFloat(0));
                LOG_INFO("Select collider: (%f, %f, %f) %p", worldPos.x, worldPos.y, worldPos.z, selectedCollider.get());

                if (selectedCollider && selectedCollider->getRigidbody()->isStatic())
                {
                    selectedCollider = nullptr;
                }
                
                if (selectedCollider)
                {
                    selectedRigidbodyPos = selectedCollider->getRigidbody()->getBodyPosition();
                }
            }
            else if (action == GLFW_RELEASE)
            {
                selectedCollider = nullptr;

                if (!autoTick)
                {
                    physics->tick(physicsDeltaTime);

                    LOG_INFO("BVH depth: %d", physics->getBVHDepth());
                    LOG_INFO("BVH node count: %d", physics->getBVHNodeCount());
                }
                
                //doRayCast();
            }
        }
        else if (button == GLFW_MOUSE_BUTTON_2)
        {
            if (action == GLFW_PRESS)
            {
                Vector2f mosuePos = getCursorPos();
                Vector3f worldPos = screenPosToWorld(mosuePos);

                FVector3 fPos(FFloat(worldPos.x), FFloat(0), FFloat(worldPos.y));

                auto collider = physics->pointCast(fPos, FFloat(0));
                if (collider != nullptr)
                {
                    auto r = collider->getRigidbody();
                    FVector3 normal = r->getBodyPosition() - fPos;
                    normal.normalize();
                    r->setBodyVelocity(r->getBodyVelocity() + normal * FFloat(5));
                }
            }
        }
        else if (button == GLFW_MOUSE_BUTTON_3)
        {
            if (action == GLFW_PRESS)
            {
                lastDragPos = getCursorPos();
                lastCameraPos = camera.getPosition();
            }
        }
    }

    void onMouseScroll(double xoffset, double yoffset) override
    {
        camera.handleMouseScroll(xoffset, yoffset * 10);
    }

    void onMouseMove(double x, double y) override
    {
#if 1
        if (selectedCollider && glfwGetMouseButton(pWindow_, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
        {
            Vector3f worldPos = screenPosToWorld(getCursorPos());
            Vector3f delta = worldPos - selectedWorldPos;
            FVector3 pos = selectedRigidbodyPos + FVector3(FFloat(delta.x), FFloat(0), FFloat(delta.y));
            selectedCollider->getRigidbody()->setBodyPosition(pos);
        }

#endif
    }

    void onTick(float deltaTime) override
    {
        if (autoTick)
        {
            physics->tick(physicsDeltaTime);
        }

        camera.handleCameraMove();
        doRayCast();
    }

    void onDraw() override
    {
        Application::onDraw();

#if 1
        physics->debugDraw();
#else
        DebugDraw::getInstance()->drawLine(FVector2::ZERO, FVector2(FFloat(-2), FFloat(1)), Color::red);
        DebugDraw::getInstance()->drawRect(FVector2::ZERO, FVector2(FFloat(2), FFloat(1)), Color::red);
        DebugDraw::getInstance()->drawSolidCircle(FVector2(FFloat(0), FFloat(2)), FFloat(1), Color::red);
#endif
        DebugDraw::getInstance()->drawLine(rayCast.start, rayCast.end, Color::blue);
        
        DebugRenderer::worldViewProjMatrix = camera.getViewProjMatrix();
        DebugRenderer::DrawDebugData();

        DebugDraw::getInstance()->clear();
    }

    FRigidbody *createCircleRigidbody(const FVector2 &position, FFloat radius)
    {
        FRigidbody* rigidbody = new FRigidbody(FFloat(1), FFloat(1));
        rigidbody->retain();
        rigidbody->setBodyPosition(FVector3::FromXZ(position));

        FCircleCollider *collider = new FCircleCollider(radius);
        rigidbody->addCollider(collider);

        physics->addRigidbody(rigidbody);
        
        return rigidbody;
    }

    FRigidbody *createPolyRigidbody(const FVector2 &position, FFloat radius)
    {
        const int n = 6;
        FVector2 vertices[n];
        for (int i = 0; i < n; ++i)
        {
            FFloat angle = FFloat(i * 360) / n;
            FVector2 normal = FVector2::fromAngle(angle);
            vertices[i] = normal * radius;
        }
        return createPolyRigidbody(position, vertices, n);
    }

    FRigidbody *createPolyRigidbody(const FVector2 &position, int *values, size_t n)
    {
        std::vector<FVector2> vertices;
        for (size_t i = 0; i < n; ++i)
        {
            size_t k = i * 2;
            vertices.push_back(FVector2(FFloat(true, values[k]), FFloat(true, values[k + 1])));
        }
        return createPolyRigidbody(position, vertices.data(), n);
    }

    FRigidbody *createPolyRigidbody(const FVector2 &position, FVector2 *vertices, size_t n)
    {
        FRigidbody* rigidbody = new FRigidbody(FFloat(1), FFloat(1));
        rigidbody->retain();
        rigidbody->setBodyPosition(FVector3::FromXZ(position));
        
        FPolygonCollider *collider = new FPolygonCollider();
        collider->setVertices(vertices, n);
        rigidbody->addCollider(collider);
        
        physics->addRigidbody(rigidbody);
        return rigidbody;
    }

    void addEdgeCollider(FFloat x1, FFloat y1, FFloat x2, FFloat y2)
    {
        FSegmentCollider *ret = new FSegmentCollider(FVector3(x1, FFloat(0), y1), FVector3(x2, FFloat(0), y2));
        physics->getStaticRigidbody()->addCollider(ret);
    }

    float rand01()
    {
        return (float)std::rand() / float(RAND_MAX);
    }

    float rand11()
    {
        return rand01() * 2 - 1;
    }

    void createPhysicsWorld()
    {
        physics = new FPhysics2D();
        physics->init();

        bool usePhysicsFile = !physicsFilePath.empty();
        if (usePhysicsFile)
        {
            //physics->loadPhysicsFromFile(physicsFilePath);

            createCircleRigidbody(FVector2(FFloat(-2), FFloat(0)), FFloat(1));

            //int vertices[] = { 1340, -19093, 1340, -14440, -4768, -14440, -4768, -19093, };
            //auto body = createPolyRigidbody(FVector2::ZERO, vertices, 4);
            //body->setStatic(true);
        }
        else
        {
            int size = 8;
            int n = 0;

            // left
            addEdgeCollider(FFloat(-size), FFloat(-size), FFloat(-size), FFloat(size));
            // right
            addEdgeCollider(FFloat(size), FFloat(-size), FFloat(size), FFloat(size));
            // bottom
            addEdgeCollider(FFloat(-size), FFloat(-size), FFloat(size), FFloat(-size));
            // top
            addEdgeCollider(FFloat(-size), FFloat(size), FFloat(size), FFloat(size));

#if 0
            n = 10;
#else
            createCircleRigidbody(FVector2(FFloat(-2), FFloat(0)), FFloat(1));
            createPolyRigidbody(FVector2(FFloat(3), FFloat(0)), FFloat(2));
#endif

            // 创建一个静态物体
            FRigidbody* r = createPolyRigidbody(FVector2(FFloat(3), FFloat(-2)), FFloat(-1));
            r->setStatic(true);

            // 创建一个Kinematic物体
            r = createPolyRigidbody(FVector2(FFloat(3), FFloat(-5)), FFloat(-1));
            r->setKinematic(true);

            // 创建一个触发器
            r = createPolyRigidbody(FVector2(FFloat(-3), FFloat(-3)), FFloat(-1));
            r->getCollider(0)->setTrigger(true);

            size -= 1;

            for (int i = 0; i < n; ++i)
            {
                FVector2 pos;
                pos.x = FFloat(rand11() * size);
                pos.y = FFloat(rand11() * size);

                FFloat radius(rand01() + 0.5f);

                FVector3 velocity;
                velocity.x = FFloat(rand11() * 10);
                velocity.z = FFloat(rand11() * 10);

                FRigidbody* ret;
                if (i % 2 == 0)
                {
                    ret = createCircleRigidbody(pos, radius);
                }
                else
                {
                    ret = createPolyRigidbody(pos, radius);
                }
                ret->setBodyVelocity(velocity);

                ret->setAngleVelociy(FFloat(rand01() * 360));

                rigidbodies.push_back(ret);
            }
        }

        LOG_INFO("BVH depth: %d", physics->getBVHDepth());
        LOG_INFO("BVH node count: %d", physics->getBVHNodeCount());
    }

    void doRayCast()
    {
        Vector3f worldPos = screenPosToWorld(getCursorPos());
        
        FVector3 start;
        FVector3 end(FFloat(worldPos.x), FFloat(0), FFloat(worldPos.y));
        if (end.isZero())
        {
            end.x = FFloat(1);
        }
        end.normalize();
        end *= FFloat(20);
        
        FColliderFilter filter;
        FRaycastHit hit;
        if (physics->linecast(start, end, FFloat(0), filter, hit))
        {
            end = hit.point;
            //LOG_DEBUG("raycast success.");
        }
        else
        {
            //LOG_DEBUG("raycast failed.");
        }
        
        rayCast.set(start.toXZ(), end.toXZ());
    }
};

NS_FXP_BEGIN
FXP_API void testFMath();
NS_FXP_END

int main(int argc, char **argv)
{
    LOG_INFO(USAGE);

    testFMath();
    reportTest();

    MainApp app;

    if (argc > 1)
    {
        app.physicsFilePath = argv[1];
    }

    if (app.createWindow(640, 480, "TestPhysics"))
    {
        app.mainLoop();
    }

    Profiler::getDefault()->endAll();
    LOG_INFO("profiler report: \n%s", Profiler::getDefault()->report().c_str());
    return 0;
}
