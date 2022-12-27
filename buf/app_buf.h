#pragma once
#ifndef __APP_BUF__H__
#define __APP_BUF__H__

#include "../com/com.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#ifdef __cplusplus
;
extern "C"
{
#endif //_cplusplu
    typedef struct __BUF__
    {
        bytes header;
        bytes body;
    } resp_bufer;
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //!__APP_BUF__H__