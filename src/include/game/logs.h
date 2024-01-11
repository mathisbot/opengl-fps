#pragma once


#include <stdarg.h>
#include <stdio.h>

#define TRACE_LOGS 6
#define DEBUG_LOGS 5
#define INFO_LOGS  4
#define WARN_LOGS  3
#define ERROR_LOGS 2
#define FATAL_LOGS 1

// Change this to change the log level
// ERROR and FATAL are always enabled
#define LOGS_LEVEL TRACE_LOGS


int fprintf_log(FILE *stream, char* file, int line, int level, const char *format, ...);


#if LOGS_LEVEL >= TRACE_LOGS
#define LOG_TRACE(...) fprintf_log(stdout, __FILE__, __LINE__, 6, __VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif

#if LOGS_LEVEL >= DEBUG_LOGS
#define LOG_DEBUG(...) fprintf_log(stdout, __FILE__, __LINE__, 5, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if LOGS_LEVEL >= INFO_LOGS
#define LOG_INFO(...) fprintf_log(stdout, __FILE__, __LINE__, 4, __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LOGS_LEVEL >= WARN_LOGS
#define LOG_WARN(...) fprintf_log(stdout, __FILE__, __LINE__, 3, __VA_ARGS__)
#else
#define LOG_WARN(...)
#endif

#define LOG_ERROR(...) fprintf_log(stderr, __FILE__, __LINE__, 2, __VA_ARGS__)
#define LOG_FATAL(...) fprintf_log(stderr, __FILE__, __LINE__, 1, __VA_ARGS__)
