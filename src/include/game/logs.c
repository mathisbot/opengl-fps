#include "logs.h"


static const char* log_levels[6] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};


int fprintf_log(FILE *stream, char* file, int line, int level, const char *format, ...)
{
    va_list args;
    int result;

    fprintf(stream, "%s::%s:%d: ", log_levels[level], file, line);

    va_start(args, format);
    result = vfprintf(stream, format, args);
    va_end(args);

    return result;
}
