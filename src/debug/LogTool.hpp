#pragma once

#include "common/FConfig.hpp"

#define LOG_LEVEL_VERBOSE    0
#define LOG_LEVEL_DEBUG      1
#define LOG_LEVEL_INFO       2
#define LOG_LEVEL_NOTICE     3
#define LOG_LEVEL_WARN       4
#define LOG_LEVEL_ERROR      5 // 错误。不需要上报，可恢复
#define LOG_LEVEL_EXCEPTION  6 // 异常。需要上报，可恢复
#define LOG_LEVEL_FATAL      7 // 致命错误，即将宕机


//define default compile level
#ifndef LOG_COMPILE_LEVEL
#   ifdef DEBUG
#       define LOG_COMPILE_LEVEL LOG_LEVEL_VERBOSE
#   else
#       define LOG_COMPILE_LEVEL LOG_LEVEL_INFO
#   endif
#endif //LOG_COMPILE_LEVEL

FXP_API void ls_logFormat(int moduleLevel, int logLevel, const char * tag, const char *format, ...);

//} 针对当前线程的配置

// default log level and tag.
namespace
{
    const int s_logLevel = LOG_LEVEL_VERBOSE;
    const char * s_logTag = "Game";
}

// define log level to cover default level.
#define DEFINE_LOG_COMPONENT(LEVEL, TAG)      \
    static const int s_logLevel = LEVEL;  \
    static const char * s_logTag = TAG


#define LOG_WITH_LEVEL(LEVEL, FORMAT, ...) \
    ls_logFormat(::s_logLevel, LEVEL, ::s_logTag, FORMAT, ##__VA_ARGS__)


#if LOG_LEVEL_FATAL >= LOG_COMPILE_LEVEL
#   define LOG_FATAL(FORMAT, ...)  LOG_WITH_LEVEL(LOG_LEVEL_FATAL, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_FATAL(FORMAT, ...)
#endif

#if LOG_LEVEL_EXCEPTION >= LOG_COMPILE_LEVEL
#   define LOG_EXCEPTION(FORMAT, ...)  LOG_WITH_LEVEL(LOG_LEVEL_EXCEPTION, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_EXCEPTION(FORMAT, ...)
#endif

#if LOG_LEVEL_ERROR >= LOG_COMPILE_LEVEL
#   define LOG_ERROR(FORMAT, ...)  LOG_WITH_LEVEL(LOG_LEVEL_ERROR, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_ERROR(FORMAT, ...)
#endif

#if LOG_LEVEL_WARN >= LOG_COMPILE_LEVEL
#   define LOG_WARN(FORMAT, ...)   LOG_WITH_LEVEL(LOG_LEVEL_WARN, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_WARN(FORMAT, ...)
#endif

#if LOG_LEVEL_NOTICE >= LOG_COMPILE_LEVEL
#   define LOG_NOTICE(FORMAT, ...) LOG_WITH_LEVEL(LOG_LEVEL_NOTICE, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_NOTICE(FORMAT, ...)
#endif

#if LOG_LEVEL_INFO >= LOG_COMPILE_LEVEL
#   define LOG_INFO(FORMAT, ...)   LOG_WITH_LEVEL(LOG_LEVEL_INFO, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_INFO(FORMAT, ...)
#endif

#if LOG_LEVEL_DEBUG >= LOG_COMPILE_LEVEL
#   define LOG_DEBUG(FORMAT, ...)  LOG_WITH_LEVEL(LOG_LEVEL_DEBUG, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_DEBUG(FORMAT, ...)
#endif

#if LOG_LEVEL_VERBOSE >= LOG_COMPILE_LEVEL
#   define LOG_VERBOSE(FORMAT, ...)  LOG_WITH_LEVEL(LOG_LEVEL_VERBOSE, FORMAT, ##__VA_ARGS__)
#else
#   define LOG_VERBOSE(FORMAT, ...)
#endif
