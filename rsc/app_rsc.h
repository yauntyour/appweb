#pragma once
#ifndef __APP_RSC__H__
#define __APP_RSC__H__

//#include "../acc/app_acc.h"
#include "../com/com.h"

#include "mime_type.h"
#include "request.h"
//#include "inc_zlib.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
    int app_on(acc_event *ev, urlc_t urlc[], size_t len)
    {
        if (ev->line_length == 0)
        {
            ev->line = (urlc_t *)calloc(len, sizeof(urlc_t));
            for (size_t i = 0; i < len; i++)
            {
                if (urlc[i].req_model < 0 || urlc[i].req_model > 8)
                {
                    urlc[i].req_model = 9;
                }
                ev->line[i] = urlc[i];
            }
        }
        else
        {
            urlc_t *temp = (urlc_t *)calloc(len + ev->line_length, sizeof(urlc_t));
            for (size_t i = 0; i < ev->line_length; i++)
            {
                temp[i] = ev->line[i];
            }
            for (size_t i = 0; i < len; i++)
            {
                temp[ev->line_length + i] = urlc[i];
            }
            free(ev->line);
            ev->line = temp;
        }
        ev->line_length += len;
        return 0;
    }
    typedef struct __search__arg
    {
        urlc_t *line;
        size_t line_length;
        req_t *request;
    } __search__arg_t;

    void *__search__(void *arg)
    {
        __search__arg_t *a = (__search__arg_t *)arg;

        bytes_create(&(a->request->data), MAX_RECV_BUF);
        recv(a->request->addr.socket, a->request->data.data, MAX_RECV_BUF, 0);
        if(req_create(a->request) == -1)
        {
            rsp_404(a->request);
        }
        for (size_t i = 0; i < a->line_length; i++)
        {
            if (a->line[i].req_model != -1)
            {
                if (a->request->url != NULL && strcmp(a->line[i].url, a->request->url) == 0)
                {
                    if (a->request->req_model != NULL && strcmp(a->request->req_model, Type_req[a->line[i].req_model]) == 0)
                    {
                        a->line[i]._func_(a->request);
                    }
                    else
                    {
                        goto __rsp404__;
                    }
                }
                else
                {
                    goto __rsp404__;
                }
            }
            else
            {
            __rsp404__:
                rsp_404(a->request);
            }
        }
        a->request->_stat_ = stat_acc;
        pthread_exit(NULL);
    }

    void *_rsc(void *arg)
    {
        acc_event *ev = (acc_event *)arg;
        while (1)
        {
            for (size_t i = 0; i < MAX_CONN; i++)
            {
                if (ev->acc_list[i]._stat_ == stat_rsc && ev->acc_list[i]._stat_ != stat_deal)
                {
                    ev->acc_list[i]._stat_ = stat_deal;
                    pthread_t t;
                    __search__arg_t a;
                    a.line = ev->line;
                    a.line_length = ev->line_length;
                    a.request = &(ev->acc_list[i]);
                    pthread_create(&t, NULL, __search__, &a);
                    pthread_detach(t);
                }
            }
        }
        pthread_exit(NULL);
    }
    int app_rsc(pthread_t *rsc_thread, acc_event *event)
    {
        return pthread_create(rsc_thread, NULL, _rsc, event);
    }
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //!__APP_RSC__H__