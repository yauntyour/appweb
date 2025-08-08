#pragma once
#ifndef __COM__H__
#define __COM__H__
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "Varde.h"
#include "metadata.h"

#define __SERVER__ "Appweb/3.0"

#define MAX_RECV_BUF 1024 // maximum 788 chars

#define MAX_TIME_LEN 32

#define TCP_CONNECT 0
#define UDP_CONNECT 1
#define ALL_CONNECT 2

typedef struct appev
{
    IPv4_addr_t tcpip, udpip;
    unsigned int port;
    Varde root_dict;
    size_t MAXCONNECT;
    int UTCoffset;
    int connect_type; // 0:TCP, 1:UDP, 2:ALL
    void *(*event_func)(void *arg);
    void *arg;
    pthread_t event_th;
} appev_t;

typedef struct __search__arg
{
    metadata_type(req_t *) request;
    appev_t *appev;
    // Varde *root_dict;
    size_t *i;
    int connect_type; // 0:TCP, 1:UDP, 2:ALL
} __search__arg_t;

#endif //!__COM__H__