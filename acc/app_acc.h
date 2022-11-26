#pragma once
#ifndef __APP_ACC__H__
#define __APP_ACC__H__

#include "../com/com.h"
#include "../rsc/app_rsc.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#ifdef __cplusplus
;
extern "C"
{
#endif //_cplusplu

    int app_event_init(appev_t *event, size_t MAXCONN)
    {
        (*event).tcpip.socket = socket(AF_INET, SOCK_STREAM, 0);
        (*event).udpip.socket = socket(AF_INET, SOCK_DGRAM, 0);

        set_nonblocking((*event).tcpip.socket, 1);
        set_nonblocking((*event).udpip.socket, 1);

        (*event).tcpip.address.sin_family = AF_INET;
        (*event).udpip.address.sin_family = AF_INET;

        (*event).tcpip.address.sin_port = htons((*event).port);
        (*event).udpip.address.sin_port = htons((*event).port + 1);
#ifdef _WIN32
        (*event).tcpip.address.sin_addr.S_un.S_addr = INADDR_ANY;
        (*event).udpip.address.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    (*event).tcpip.address.sin_addr.s_addr = INADDR_ANY;
    (*event).udpip.address.sin_addr.s_addr = INADDR_ANY;
#endif
        if (bind((*event).tcpip.socket, (const sockaddr *)&(*event).tcpip.address, sizeof((*event).tcpip.address)) == SOCKET_ERROR)
        {
            printf("%s: bind TCP %s:%d failed. %s\n", __func__, "localhost", (*event).port, strerror(errno));
            return -1;
        }
        if (bind((*event).udpip.socket, (const sockaddr *)&(*event).udpip.address, sizeof((*event).udpip.address)) == SOCKET_ERROR)
        {
            printf("%s: bind UDP %s:%d failed. %s\n", __func__, "localhost", (*event).port + 1, strerror(errno));
            return -1;
        }

        if (listen((*event).tcpip.socket, MAXCONN) == SOCKET_ERROR)
        {
            printf("%s: listen TCP %s:%d failed. %s\n", __func__, "localhost", (*event).port, strerror(errno));
            return -1;
        }

        (*event).MAXCONNECT = MAXCONN;

        (*event).root_dict.func = NULL;
        (*event).root_dict.Name = NULL;
        (*event).root_dict.req_Type = Type_ALL;

        (*event).root_dict.list = NULL;
        (*event).root_dict.list_length = 0;
        (*event).root_dict.list_size = 0;

        signal(SIGINT, sighandler);
        signal(SIGTERM, sighandler);

        return 0;
    }

    int app_event_free(appev_t *event)
    {
        close_socket(event->tcpip.socket);
        close_socket(event->udpip.socket);
        return 0;
    }
    void *_acc(void *arg)
    {
        appev_t *event = (appev_t *)arg;
        LOG("Server start at host:[http://localhost:%d]\n", event->port);
        size_t t = 0;
        while (1)
        {
            req_t *request = (req_t *)calloc(1,sizeof(req_t));
#ifdef _WIN32
            int sizeof_req = sizeof((*request).addr.address);
#else
        socklen_t sizeof_req = sizeof(request.addr.address);
#endif
        __Accept__:
            (*request).addr.socket = accept(event->tcpip.socket, (sockaddr *)&(*request).addr.address, &sizeof_req);
            if ((*request).addr.socket == INVALID_SOCKET)
            {
                int err = get_error();
#ifdef _WIN32
                if (err != 10035)
                {
                    LOG("%s accept Errorcode:%d %s\r\n", __func__, err, strerror(err));
                }
#else
            if (err != EAGAIN)
            {
                LOG("%s accept Errorcode:%d %s\r\n", __func__, err, strerror(err));
            }
#endif
                goto __Accept__;
            }
            else
            {
                time(&((*request).time));
                char buf[MAX_TIME_LEN];
                LOG("[%s](%s:%d)\r\n", getTMUTC(buf, MAX_TIME_LEN, event->UTCoffset, "%a %b %d %X %Y", &((*request).time)), inet_ntoa((*request).addr.address.sin_addr), (*request).addr.address.sin_port);
            __Restart__:
                if (t >= event->MAXCONNECT)
                {
                    LOG("CONNECT MAX: %d\n", event->MAXCONNECT);
                    send((*request).addr.socket, "HTTP/1.1 503\r\n\0\0\0", 18, MSG_WAITALL);
                    goto __Restart__;
                }
                else
                {
                    app_rsc(&t,request,event);
                    t += 1;
                }
            }
        }
        pthread_exit(NULL);
    }
    int app_acc(pthread_t *th, appev_t *event)
    {
        return pthread_create(th, NULL, _acc, event);
    }
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //!__APP_ACC__H__