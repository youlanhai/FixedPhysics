//////////////////////////////////////////////////////////////////////
/// Desc  FConfig
/// Time  2020/12/03
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#pragma once

// 命名空间
#define NS_FXP_BEGIN namespace fxp{
#define NS_FXP_END   }
#define NS_FXP_USING using namespace fxp;

#ifdef FPHYSICS_SHARED
    #ifdef _WIN32
        #ifdef FPHYSICS_API_EXPORT
            #define FXP_API __declspec(dllexport)
        #else
            #define FXP_API __declspec(dllimport)
        #endif
    #else
        #define FXP_API __attribute__((visibility("default")))
    #endif
#else
    #define FXP_API
#endif

#include <cstddef>

#ifdef __APPLE__
#   define VARG_SAFE_CHECK(fmtIndex, vargIndex) __printflike(fmtIndex, vargIndex)
#else
#   define VARG_SAFE_CHECK(fmtIndex, vargIndex)
#endif

#define DISABLE_COPY_AND_ASSIGN(TYPE) \
    const TYPE& operator = (const TYPE &) = delete; \
    TYPE(const TYPE &) = delete
