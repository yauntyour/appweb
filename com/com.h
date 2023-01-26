#pragma once
#ifndef __COM__H__
#define __COM__H__
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "app_socket.h"
#include "bytes.h"
#include "http.h"
#include "address.h"
#include "log.h"
#include "metadata.h"

#define __SERVER__ "Appweb/2.0"

#define MAX_RECV_BUF 1024

#define MAX_TIME_LEN 32

#define stat_acc 0
#define stat_rsc 1
#define stat_deal 2

#define IOS_charset_UTF8 0xff
#define __error__ -1

#include "Varde.h"
typedef struct appev
{
    IPv4_addr_t tcpip, udpip;
    unsigned int port;
    Varde root_dict;
    size_t MAXCONNECT;
    int UTCoffset;
} appev_t;
#define FUNC_CB_C(__name__) int __name__(req_t *request)

#endif //!__COM__H__