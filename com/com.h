#pragma once
#ifndef __COM__H__
#define __COM__H__
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "Varde.h"
#include "metadata.h"

#define __SERVER__ "Appweb/2.0"

#define MAX_RECV_BUF 1024 // maximum 788 chars

#define MAX_TIME_LEN 32

typedef struct appev
{
    IPv4_addr_t tcpip, udpip;
    unsigned int port;
    Varde root_dict;
    size_t MAXCONNECT;
    int UTCoffset;
} appev_t;

#endif //!__COM__H__