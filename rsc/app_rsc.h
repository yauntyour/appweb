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
        metadata_type(req_t *) request;
        appev_t *appev;
        // Varde *root_dict;
        size_t *i;
    } __search__arg_t;

    metadata_dtorc(buf_dtor)
    {
        free(ptr);
    }

    void *__search__(void *arg)
    {
        __search__arg_t *a = ((__search__arg_t *)arg);
        int e = 0;
        bytes_create(&(metadata_ptr(a->request)->data), MAX_RECV_BUF);
        int result = 0;
        while (1)
        {

            result = recv(metadata_ptr(a->request)->addr.socket, metadata_ptr(a->request)->data.data, MAX_RECV_BUF, 0);
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
        if (req_create(metadata_ptr(a->request)) == -1)
        {
            LOG_ERR("Failed to create requestHeader at %s() Result:%d\r\n", __func__, result);
            rsp_404(metadata_ptr(a->request));
        }
        else
        {
            Varde *Temp = &(a->appev->root_dict);

            // HTTP/1.1 200 OK\r\nDate: Thu Jan 26 10:57:27 2023\r\n
            metadata_type(bytes *) buf = (metadata_type(bytes *))metadata_alloc(sizeof(bytes), buf_dtor);

            for (size_t i = 0; i < metadata_ptr(a->request)->url_slice_len; i++)
            {
                if (strlen(metadata_ptr(a->request)->url_slice[i]) == 0)
                {
                    if (i == metadata_ptr(a->request)->url_slice_len - 1)
                    {
                        if (a->appev->root_dict.func != NULL)
                        {
                            send(metadata_ptr(a->request)->addr.socket, "HTTP/1.1 200 OK\r\n", 17, 0);

                            char time_buf[MAX_TIME_LEN];
                            getTMUTC(time_buf, MAX_TIME_LEN, a->appev->UTCoffset, "%a %b %d %X %Y", &(metadata_ptr(a->request)->time));
                            send(metadata_ptr(a->request)->addr.socket, time_buf, strlen(time_buf), 0);

                            send(metadata_ptr(a->request)->addr.socket, "\r\nContent-Type: ", 16, 0);
                            send(metadata_ptr(a->request)->addr.socket, Temp->resp_mime_type, strlen(Temp->resp_mime_type), 0);
                            send(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0);

                            char *html = a->appev->root_dict.func(metadata_ptr(a->request), metadata_ptr(buf));

                            if (metadata_ptr(buf)->data != NULL)
                            {
                                send(metadata_ptr(a->request)->addr.socket, metadata_ptr(buf)->data, strlen(metadata_ptr(buf)->data), 0);
                            }

                            if (html[0] != '\0')
                            {
                                send(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0);
                                send(metadata_ptr(a->request)->addr.socket, html, strlen(html), 0);
                            }
                        }
                        else
                        {
                            goto __rsp__;
                        }
                    }
                    continue;
                }
                else
                {
                    for (size_t j = 0; j < Temp->list_length; j++)
                    {
                        if (strcmp(metadata_ptr(a->request)->url_slice[i], Temp->list[j].Name) == 0)
                        {
                            Temp = &(Temp->list[j]);
                            if (Temp->list == NULL && i < metadata_ptr(a->request)->url_slice_len - 1)
                            {
                                if (strlen(metadata_ptr(a->request)->url_slice[i + 1]) == 0)
                                {
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
                    if (i == metadata_ptr(a->request)->url_slice_len - 1)
                    {
                    __rsp__:
                        if (Temp->req_Type == Type_ALL)
                        {
                            send(metadata_ptr(a->request)->addr.socket, "HTTP/1.1 200 OK\r\n", 17, 0);

                            char time_buf[MAX_TIME_LEN];
                            getTMUTC(time_buf, MAX_TIME_LEN, a->appev->UTCoffset, "%a %b %d %X %Y\r\n", &(metadata_ptr(a->request)->time));
                            send(metadata_ptr(a->request)->addr.socket, time_buf, strlen(time_buf), 0);

                            send(metadata_ptr(a->request)->addr.socket, "\r\nContent-Type: ", 16, 0);
                            send(metadata_ptr(a->request)->addr.socket, Temp->resp_mime_type, strlen(Temp->resp_mime_type), 0);
                            send(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0);

                            char *html = Temp->func(metadata_ptr(a->request), metadata_ptr(buf));

                            if (metadata_ptr(buf)->data != NULL)
                            {
                                send(metadata_ptr(a->request)->addr.socket, metadata_ptr(buf)->data, strlen(metadata_ptr(buf)->data), 0);
                            }

                            if (html[0] != '\0')
                            {
                                send(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0);
                                send(metadata_ptr(a->request)->addr.socket, html, strlen(html), 0);
                            }

                            break;
                        }
                        else if (metadata_ptr(a->request)->req_model != NULL && strcmp(metadata_ptr(a->request)->req_model, Type_req[Temp->req_Type]) == 0)
                        {
                            send(metadata_ptr(a->request)->addr.socket, "HTTP/1.1 200 OK\r\nDate: ", 23, 0);

                            char time_buf[MAX_TIME_LEN];
                            getTMUTC(time_buf, MAX_TIME_LEN, a->appev->UTCoffset, "%a %b %d %X %Y", &(metadata_ptr(a->request)->time));
                            send(metadata_ptr(a->request)->addr.socket, time_buf, strlen(time_buf), 0);

                            send(metadata_ptr(a->request)->addr.socket, "\r\nContent-Type: ", 16, 0);
                            send(metadata_ptr(a->request)->addr.socket, Temp->resp_mime_type, strlen(Temp->resp_mime_type), 0);
                            send(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0);

                            char *html = Temp->func(metadata_ptr(a->request), metadata_ptr(buf));

                            if (metadata_ptr(buf)->data != NULL)
                            {
                                send(metadata_ptr(a->request)->addr.socket, metadata_ptr(buf)->data, strlen(metadata_ptr(buf)->data), 0);
                            }

                            if (html[0] != '\0')
                            {
                                send(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0);
                                send(metadata_ptr(a->request)->addr.socket, html, strlen(html), 0);
                            }
                            break;
                        }
                        else
                        {
                        __rsp_404__:
                            LOG_ERR("Response HTTP 404 Not Found. At %s()::errcode: %d\r\n", __func__, e);
                            rsp_404(metadata_ptr(a->request));
                            break;
                        }
                    }
                }
            }
            bytes_delete(metadata_ptr(buf));
            metadata_free(buf);
        }
        *(a->i) -= 1;

        close_socket(metadata_ptr(a->request)->addr.socket);
        metadata_free(a->request);
        free(arg);
        pthread_exit(NULL);
    }
    int app_rsc(size_t *i, metadata_type(req_t *) request, appev_t *event)
    {
        pthread_t t;
        __search__arg_t *arg = (__search__arg_t *)malloc(sizeof(__search__arg_t));

        arg->request = request;
        arg->appev = event;
        arg->i = i;

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