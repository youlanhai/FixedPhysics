#include "ProfilerKeys.hpp"
#include <unordered_map>
#include <string>

NS_FXP_BEGIN

class KeyNameMap
{
public:
    KeyNameMap()
    {
        R(PK_ROOT, "root");
        R(PK_WORLD_TICK, "world");
        R(PK_PHYSICS_TICK, "physics");
        R(PK_GAME_OBJECT_TICK, "gameObject");
        R(PK_MOTION_TICK, "motion");
        R(PK_PHYSICS_JUDGE_PAIR, "queryPair");
        R(PK_PHYSICS_PRE_SEPERATION, "preSeperation");
        R(PK_PHYSICS_POST_SEPERATION, "postSeperation");
        R(PK_PHYSICS_LINECAST, "linecast");
        R(PK_PHYSICS_COLLISION_TEST, "collisionTest");
        R(PK_PHYSICS_GJK_TEST, "gjkTest");
        R(PK_PHYSICS_GJK_ONLY, "gjk");
        R(PK_PHYSICS_EPA_ONLY, "epa");
        R(PK_PHYSICS_BVH_CHANGE, "bvhChange");
        R(PK_PHYSICS_BVH_ADD, "bvhAdd");
        R(PK_PHYSICS_BVH_REMOVE, "bvhRemove");
        R(PK_PHYSICS_BVH_REBUILD, "bvhRebuild");
        R(PK_PHYSICS_COLLIDERCAST, "colliderCast");
        R(PK_PHYSICS_NOTIFY, "notify");

        R(PK_TIMER, "timer");
        R(PK_TIMER_CALL, "timerCall");
    }

    void R(int key, const char *name)
    {
        names_[key] = name;
    }

    const std::string& get(int key) const
    {
        auto it = names_.find(key);
        if (it != names_.end())
        {
            return it->second;
        }
        return empty_;
    }

private:
    std::string empty_;
    std::unordered_map<int, std::string> names_;
};

const std::string& getProfilerKeyName(int key)
{
    static KeyNameMap map;
    return map.get(key);
}

NS_FXP_END
