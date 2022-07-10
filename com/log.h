#ifndef __LOG__H__
#define __LOG__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOGFILE(filepath, LOG_file_p) LOG_file_p = fopen(filepath, "a+")
#define LOG(format, ...)                  \
    fprintf(stdout, format, __VA_ARGS__)
#define LOGFILEOUT(LOG_file_p, format, ...)   \
    fprintf(LOG_file_p, format, __VA_ARGS__); \
    fprintf(stdout, format, __VA_ARGS__)

char *getTMUTC(char *buf,int len,int UTCoffset,const char * _Format,time_t *time)
{
    memset(buf,0,len);
    tm t;
#ifdef _WIN32
    gmtime_s(&t, time);
#else
    gmtime_r(time,&t);
#endif
    t.tm_hour += UTCoffset;
    //Sun Jun 05 01:32:50(UTC) 2022 
    if (strftime(buf, len, _Format, &t) != 0)
    {
        return buf;
    }
    else
    {
        return NULL;
    }
}
#endif //!__LOG__H__