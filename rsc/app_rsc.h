#pragma once
#ifndef __APP_RSC__H__
#define __APP_RSC__H__

// #include "../acc/app_acc.h"
#include "../com/com.h"

#include "mime_type.h"
#include "request.h"
#include "app_tcp.h"
#include "app_udp.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    void *__search__(void *arg)
    {
        __search__arg_t *a = ((__search__arg_t *)arg);
        int e = 0;
        bytes_create(&(metadata_ptr(a->request)->data), MAX_RECV_BUF);
        int result = 0;
        while (1)
        {

            if (a->connect_type == TCP_CONNECT)
            {
                result = recv(metadata_ptr(a->request)->addr.socket, metadata_ptr(a->request)->data.data, MAX_RECV_BUF, 0);
            }
            else if (a->connect_type == UDP_CONNECT)
            {
                int addr_len = sizeof(metadata_ptr(a->request)->addr.address);
                result = recvfrom(metadata_ptr(a->request)->addr.socket, metadata_ptr(a->request)->data.data, MAX_RECV_BUF, 0,
                                  (struct sockaddr *)&metadata_ptr(a->request)->addr.address, &addr_len);
            }

#ifdef _WIN32
            if (result < 0)
            {
                int err = get_error();
                if (err != 10035)
                {
                    LOG_ERR("%s recv Errorcode:%d %s\r\n", __func__, err, strerror(err));
                }
            }
            else if (result == 0)
            {
                close_socket(metadata_ptr(a->request)->addr.socket);
                metadata_free(a->request);
                free(arg);
                pthread_exit(NULL);
            }
            else
            {
                break;
            }
#else
        if (result < 0)
        {
            int err = get_error();
            LOG_ERR("%s recv Errorcode:%d %s\r\n", __func__, err, strerror(err));
        }
        else if (result == 0)
        {
            close_socket(metadata_ptr(a->request)->addr.socket);
            metadata_free(a->request);
            free(arg);
            pthread_exit(NULL);
        }

#endif
        }
        if (a->connect_type == TCP_CONNECT)
        {
            app_tcp(a, &result, &e);
        }
        else if (a->connect_type == UDP_CONNECT)
        {
            app_udp(a, &result, &e);
        }

        *(a->i) -= 1;
        close_socket(metadata_ptr(a->request)->addr.socket);
        metadata_free(a->request);
        free(arg);
        pthread_exit(NULL);
        return NULL;
    }
    int app_rsc(size_t *i, metadata_type(req_t *) request, appev_t *event, int connect_type)
    {
        pthread_t t;
        __search__arg_t *arg = (__search__arg_t *)malloc(sizeof(__search__arg_t));

        arg->request = request;
        arg->appev = event;
        arg->i = i;
        arg->connect_type = connect_type;

        if (pthread_create(&t, NULL, __search__, arg))
        {
            LOG_ERR("Thread created fail at %s()\r\n", t, __func__);
        }
        pthread_detach(t);
        return 0;
    }
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //!__APP_RSC__H__
