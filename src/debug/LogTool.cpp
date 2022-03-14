#include "LogTool.hpp"
#include <cstdarg>
#include <cstdio>

static const char * LogLevelString[] = {
    "VERBOSE",
    "DEBUG",
    "INFO",
    "NOTICE",
    "WARN",
    "ERROR",
    "EXCEPTION",
    "FATAL"
};

FXP_API void ls_logFormat(int moduleLvl, int logLevel, const char * tag, const char *format, ...)
{
    if(logLevel < moduleLvl)
    {
        return;
    }

    printf("[%s]%s: ", LogLevelString[logLevel], tag);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}
