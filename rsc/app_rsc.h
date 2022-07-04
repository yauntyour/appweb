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
        if ((*ev).line == NULL)
        {
            (*ev).line = (urlc_t *)calloc(len, sizeof(urlc_t));
            for (size_t i = 0; i < len; i++)
            {
                for (size_t j = 0; j <= 8; i++)
                {
                    if (urlc[i].req_model == j)
                    {
                        (*ev).line[i] = urlc[i];
                        break;
                    }
                    else if (j == 8)
                    {
                        urlc[i].req_model = -1;
                        (*ev).line[i] = urlc[i];
                        break;
                    }
                }
            }
            (*ev).line_length = len;
            return 0;
        }
        else
        {
            urlc_t *temp = (urlc_t *)calloc(len + (*ev).line_length, sizeof(urlc_t));

            for (size_t i = 0; i < (*ev).line_length; i++)
            {
                temp[i] = (*ev).line[i];
            }

            for (size_t i = 0; i < len; i++)
            {
                temp[i + (*ev).line_length] = urlc[i];
            }

            (*ev).line_length += len;
            (*ev).line = temp;
            return 0;
        }
    }

    void *__search__(void *req)
    {
        req_t *request = (req_t *)req;

        bytes_create(&(request->data), MAX_RECV_BUF);
        recv(request->addr.socket, request->data.data, MAX_RECV_BUF, 0);
        req_create(request);
        
        printf(request->url);

        return NULL;
    }

    int app_rsc(acc_event *ev)
    {
        while (1)
        {
            for (size_t i = 0; i < MAX_CONN; i++)
            {
                if (ev->acc_list[i]._stat_ == stat_rsc)
                {
                    pthread_t t;
                    pthread_create(&t,NULL,__search__,&(ev->acc_list[i]));
                    pthread_join(t,NULL);
                }
            }
        }
    }

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //!__APP_RSC__H__