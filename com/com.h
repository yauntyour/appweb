#pragma once
#ifndef __COM__H__
#define __COM__H__
#include "app_socket.h"
#include "bytes.h"
#include "http.h"
#include "address.h"
#include "log.h"

#define __SERVER__ "Appweb/1.0"

#define HOME_DIR "./home/"

#define MAX_CONN 128
#define MAX_RECV_BUF 8 * 1024
#define MAX_LINE 32
#define MAX_TIME_LEN 32

#define stat_acc 0
#define stat_rsc 1

#define IOS_charset_UTF8 0xff
#define  __error__  -1

typedef struct request_event
{
    time_t time;
    IPv4_addr_t addr;
    bytes data;
    char *reqline[MAX_LINE];
    char *req_model;
    char *url;
    int _stat_;
} req_t;

typedef int (*func_cb)(req_t *);

/*
necessary arg:
char* url -URL path with on();
func_cb _func_ -deal by function;
int req_model,
    rsp_code;
*/
typedef struct urlc
{
    char *url;
    func_cb _func_;
    int req_model,
        rsp_code;
} urlc_t;

typedef struct acc_event
{
    IPv4_addr_t tcpip, udpip;
    unsigned int port;
    req_t acc_list[MAX_CONN];
    size_t line_length;
    urlc_t *line;
    int UTCoffset;
} acc_event;

#ifndef __APP_ACC__H__
#include "../acc/app_acc.h"
#endif //!__APP_ACC__H__
#ifndef __APP_RSC__H__
#include "../rsc/app_rsc.h"
#endif //!__APP_RES__H__

#ifndef __APP_RES__H__
#include "../res/app_res.h"
#endif //!__APP_RES__H__

#define FUNC_CB_C(__name__) int __name__(req_t *request)

#endif //!__COM__H__