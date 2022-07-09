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
#define MAX_RECV_BUF 2048
#define MAX_LINE 32

#define MAX_TIME_LEN 32

#define stat_acc 0
#define stat_rsc 1
#define stat_deal 2

#define IOS_charset_UTF8 0xff
#define __error__ -1

#include <signal.h>

void sighandler(int signum)
{
   printf("signal code:%d,over\n", signum);
   exit(signum);
}

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
if req_model == -1,
*/
typedef struct urlc
{
    char *url;
    func_cb _func_;
    int req_model;
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
#define FUNC_CB_C(__name__) int __name__(req_t *request)

#endif //!__COM__H__