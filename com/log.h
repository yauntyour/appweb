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
    gmtime_s(&t, time);
    t.tm_hour += UTCoffset;
    //Sun Jun 05 01:32:50(UTC) 2022 
    strftime(buf,len,_Format,&t);
    return buf;
}
#endif //!__LOG__H__