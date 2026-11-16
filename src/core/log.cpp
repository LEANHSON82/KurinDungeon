#include "core/log.h"
#include <cstdarg>
#include <cstdio>

static void PrintToStderr(LogLevel level, const char *message)
{
    std::fprintf(stderr, "%s: %s\n", level == LogLevel::Warning ? "WARNING" : "INFO", message);
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
