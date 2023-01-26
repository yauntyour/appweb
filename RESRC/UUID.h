#ifndef __UUID__H__
#define __UUID__H__
#include <stdio.h>
#include <time.h>
#include "sm3.h"

typedef long long UUID_t;
#define BUF_LEN 32 + 24 + 16
char *UUID_getTMUTC(char *buf, int len, const char *_Format, time_t *time)
{
    memset(buf, 0, len);
    struct tm t;
#ifdef _WIN32
    gmtime_s(&t, time);
#else
    gmtime_r(time, &t);
#endif
    // Sun Jun 05 01:32:50(UTC) 2022
    if (strftime(buf, len, _Format, &t) != 0)
    {
        return buf;
    }
    else
    {
        return NULL;
    }
}
/*UUID length of 32*/
int UUID_create(unsigned char* hash,unsigned int hash_len,UUID_t *time_diff)
{
    char buf[BUF_LEN];

    time_t t;
    time(&t);
    sprintf(buf, "%s %d", UUID_getTMUTC(buf, BUF_LEN, "%a %b %d %X %Y", &t), (*time_diff));

    sm3((unsigned char *)buf,BUF_LEN,hash,&hash_len);

    (*time_diff) += 1;
    return 0;
}
#endif //!__UUID__H__