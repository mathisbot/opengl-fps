#pragma once


#include <stdarg.h>
#include <stdio.h>

#define TRACE_LOGS 5
#define DEBUG_LOGS 4
#define INFO_LOGS  3
#define WARN_LOGS  2
#define ERROR_LOGS 1
#define FATAL_LOGS 0

// Change this to change the log level
// ERROR and FATAL are always enabled
#ifndef LOGS_LEVEL
#define LOGS_LEVEL TRACE_LOGS
#endif


int fprintf_log(FILE *stream, char* file, int line, int level, const char *format, ...);


#if LOGS_LEVEL >= TRACE_LOGS
#define LOG_TRACE(...) fprintf_log(stdout, __FILE__, __LINE__, TRACE_LOGS, __VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif

#if LOGS_LEVEL >= DEBUG_LOGS
#define LOG_DEBUG(...) fprintf_log(stdout, __FILE__, __LINE__, DEBUG_LOGS, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if LOGS_LEVEL >= INFO_LOGS
#define LOG_INFO(...) fprintf_log(stdout, __FILE__, __LINE__, INFO_LOGS, __VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LOGS_LEVEL >= WARN_LOGS
#define LOG_WARN(...) fprintf_log(stdout, __FILE__, __LINE__, WARN_LOGS, __VA_ARGS__)
#else
#define LOG_WARN(...)
#endif

#define LOG_ERROR(...) fprintf_log(stderr, __FILE__, __LINE__, ERROR_LOGS, __VA_ARGS__)
#define LOG_FATAL(...) fprintf_log(stderr, __FILE__, __LINE__, FATAL_LOGS, __VA_ARGS__)
