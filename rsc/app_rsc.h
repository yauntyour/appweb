#pragma once
#ifndef __APP_RSC__H__
#define __APP_RSC__H__

// #include "../acc/app_acc.h"
#include "../com/com.h"

#include "mime_type.h"
#include "request.h"
// #include "inc_zlib.h"

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
        size_t *i;
    } __search__arg_t;

    void *__search__(void *arg)
    {
        __search__arg_t a = *((__search__arg_t *)arg);
        int e = 0;
        bytes_create(&(a.request->data), MAX_RECV_BUF);
        recv(a.request->addr.socket, a.request->data.data, MAX_RECV_BUF, 0);
        if (req_create(a.request) == -1)
        {
            LOG_ERR("Failed to create at %s\r\n", __func__);
            rsp_404(a.request);
        }
        else
        {
            Varde *Temp = a.root_dict;
            for (size_t i = 0; i < a.request->url_slice_len; i++)
            {
                if (strlen(a.request->url_slice[i]) == 0)
                {
                    if (i == a.request->url_slice_len - 1)
                    {
                        if (a.root_dict->func != NULL)
                        {
                            a.root_dict->func(a.request);
                        }
                        else
                        {
                            e = 1;
                            goto __rsp__;
                        }
                    }
                    continue;
                }
                else
                {
                    for (size_t j = 0; j < Temp->list_length; j++)
                    {
                        if (strcmp(a.request->url_slice[i], Temp->list[j].Name) == 0)
                        {
                            Temp = &(Temp->list[j]);
                            if (Temp->list == NULL && i < a.request->url_slice_len - 1)
                            {
                                if (strlen(a.request->url_slice[i + 1]) == 0)
                                {
                                    e = 2;
                                    goto __rsp__;
                                }
                                e = 3;
                                goto __rsp_404__;
                            }
                            break;
                        }
                        else if (j == Temp->list_length - 1)
                        {
                            if (Temp->ComPath == ComPath_True)
                            {
                                e = 4;
                                goto __rsp__;
                            }
                            else
                            {
                                e = 5;
                                goto __rsp_404__;
                            }
                        }
                    }
                    if (i == a.request->url_slice_len - 1)
                    {
                    __rsp__:
                        if (Temp->req_Type == Type_ALL)
                        {
                            Temp->func(a.request);
                            break;
                        }
                        else if (a.request->req_model != NULL && strcmp(a.request->req_model, Type_req[Temp->req_Type]) == 0)
                        {
                            Temp->func(a.request);
                            break;
                        }
                        else
                        {
                        __rsp_404__:
                            LOG_ERR("Not Found. at %s %d\r\n", __func__, e);
                            rsp_404(a.request);
                            break;
                        }
                    }
                }
            }
        }
        *(a.i) -= 1;

        bytes_delete(&(a.request->data));
        mem_free(a.request->reqline);
        mem_free(a.request->url_slice);
        mem_free(a.request);
        mem_free(arg);
        /*
        free(a.request->data.data);
        free(a.request->reqline);
        free(a.request->url_slice);
        a.request->data.data = NULL;
        a.request->reqline = NULL;
        a.request->url_slice = NULL;

        free(a.request);
        a.request = NULL;
*/
        pthread_exit(NULL);
    }
    int app_rsc(size_t *i, req_t *request, appev_t *event)
    {
        pthread_t t;
        __search__arg_t *arg = (__search__arg_t *)malloc(sizeof(__search__arg_t));

        arg->request = request;
        arg->root_dict = &(event->root_dict);
        arg->i = i;

        if (pthread_create(&t, NULL, __search__, arg))
        {
            LOG_ERR("Thread[PID:%d] created fail at %s()\r\n", t, __func__);
        }
        else
        {
            LOG_INFO("Thread[PID:%d] created at %s()\r\n", t, __func__);
        }

        pthread_detach(t);
        return 0;
    }
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //!__APP_RSC__H__