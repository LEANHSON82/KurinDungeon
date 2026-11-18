#pragma once

enum class LogLevel
{
    Debug,
    Info,
    Warning,
};

using LogSink = void (*)(LogLevel level, const char *message);

void SetLogSink(LogSink sink);

#if defined(__GNUC__)
__attribute__((format(printf, 2, 3)))
#endif
void Log(LogLevel level, const char *format, ...);
