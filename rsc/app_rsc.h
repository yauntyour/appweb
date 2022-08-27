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
    typedef struct __search__arg
    {
        req_t *request;
        Varde *root_dict;
    } __search__arg_t;
    void *__search__(void *arg)
    {
        __search__arg_t *a = (__search__arg_t *)arg;
        bytes_create(&(a->request->data), MAX_RECV_BUF);
        recv(a->request->addr.socket, a->request->data.data, MAX_RECV_BUF, 0);
        if (req_create(a->request) == -1)
        {
            rsp_404(a->request);
        }
        else
        {
            Varde *Temp = a->root_dict;
            for (size_t i = 0; i < a->request->url_slice_len; i++)
            {
                if (strlen(a->request->url_slice[i]) == 0)
                {
                    if (i == a->request->url_slice_len - 1)
                    {
                        if (a->root_dict->func != NULL)
                        {
                            a->root_dict->func(a->request);
                        }
                        else
                        {
                            rsp_404(a->request);
                        }
                    }
                    continue;
                }
                else
                {
                    for (size_t j = 0; j < Temp->list_length; j++)
                    {
                        if (strcmp(a->request->url_slice[i], Temp->list[j].Name) == 0)
                        {
                            Temp = &(Temp->list[j]);
                            if (Temp->list == NULL && i < a->request->url_slice_len - 1)
                            {
                                if (strlen(a->request->url_slice[i + 1]) == 0)
                                {
                                    goto __rsp__;
                                }
                                goto __rsp_404__;
                            }
                            break;
                        }
                        else if (j == Temp->list_length - 1)
                        {
                            if (Temp->ComPath == ComPath_True)
                            {
                                goto __rsp__;
                            }
                            else
                            {
                                goto __rsp_404__;
                            }
                        }
                    }
                    if (i == a->request->url_slice_len - 1)
                    {
                    __rsp__:
                        if (Temp->req_Type == Type_ALL)
                        {
                            Temp->func(a->request);
                            break;
                        }
                        else if (a->request->req_model != NULL && strcmp(a->request->req_model, Type_req[Temp->req_Type]) == 0)
                        {
                            Temp->func(a->request);
                            break;
                        }
                        else
                        {
                        __rsp_404__:
                            rsp_404(a->request);
                            break;
                        }
                    }
                }
            }
        }
        realloc(a->request, sizeof(req_t));
        a->request->_stat_ = stat_acc;
        pthread_exit(NULL);
        return NULL;
    }
    void *_rsc(void *arg)
    {
        appev_t *ev = (appev_t *)arg;
        while (1)
        {
            for (size_t i = 0; i < ev->Thread_queue_length; i++)
            {
                if (ev->Thread_queue[i]._stat_ == stat_rsc && ev->Thread_queue[i]._stat_ != stat_deal)
                {
                    ev->Thread_queue[i]._stat_ = stat_deal;
                    pthread_t t;
                    __search__arg_t arg;
                    arg.request = &(ev->Thread_queue[i]);
                    arg.root_dict = &(ev->root_dict);
                    pthread_create(&t, NULL, __search__, &arg);
                    pthread_detach(t);
                }
            }
        }
        pthread_exit(NULL);
        return NULL;
    }
    int app_rsc(pthread_t *rsc_thread, appev_t *event)
    {
        return pthread_create(rsc_thread, NULL, _rsc, event);
    }
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //!__APP_RSC__H__