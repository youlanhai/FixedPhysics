//////////////////////////////////////////////////////////////////////
/// Desc  ProfilerKeys
/// Time  2021/09/28
/// Autor youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once
#include "common/FConfig.hpp"

NS_FXP_BEGIN

enum ProfilerKey
{
    PK_ROOT = 1,
    PK_WORLD_TICK = 2,
    PK_PHYSICS_TICK = 3,
    PK_GAME_OBJECT_TICK = 4,
    PK_MOTION_TICK = 5,

    PK_PHYSICS_JUDGE_PAIR = 6,
    PK_PHYSICS_PRE_SEPERATION = 7,
    PK_PHYSICS_POST_SEPERATION = 8,
    PK_PHYSICS_LINECAST = 10,
    PK_PHYSICS_COLLISION_TEST = 11,
    PK_PHYSICS_GJK_TEST = 12,
    PK_PHYSICS_GJK_ONLY = 13,
    PK_PHYSICS_EPA_ONLY = 14,
    PK_PHYSICS_BVH_CHANGE = 15,
    PK_PHYSICS_BVH_ADD = 16,
    PK_PHYSICS_BVH_REMOVE = 17,
    PK_PHYSICS_BVH_REBUILD = 18,
    PK_PHYSICS_COLLIDERCAST = 19,
    PK_PHYSICS_NOTIFY = 20,

    PK_TIMER = 50,
    PK_TIMER_CALL = 51,

    /* 第三方库从这里开始定义 */
    PK_CUSTOM = 100000,
};

NS_FXP_END
