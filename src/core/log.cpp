#include "core/log.h"
#include <cstdarg>
#include <cstdio>

static const char *GetLevelName(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARNING";
    }
    return "INFO";
}

static void PrintToStderr(LogLevel level, const char *message)
{
    std::fprintf(stderr, "%s: %s\n", GetLevelName(level), message);
}

static LogSink currentSink = PrintToStderr;

void SetLogSink(LogSink sink)
{
    currentSink = sink != nullptr ? sink : PrintToStderr;
}

void Log(LogLevel level, const char *format, ...)
{
    char message[512];
    va_list args;
    va_start(args, format);
    std::vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    currentSink(level, message);
}
