#pragma once
#ifndef __APP_ACC__H__
#define __APP_ACC__H__
#include "../com/com.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#ifdef __cplusplus
;
extern "C"
{
#endif //_cplusplu

    int app_event_init(acc_event *event)
    {
        (*event).tcpip.socket = socket(AF_INET, SOCK_STREAM, 0);
        (*event).udpip.socket = socket(AF_INET, SOCK_DGRAM, 0);

        set_nonblocking((*event).tcpip.socket, 1);
        set_nonblocking((*event).udpip.socket, 1);

        (*event).tcpip.address.sin_family = AF_INET;
        (*event).udpip.address.sin_family = AF_INET;

        (*event).tcpip.address.sin_port = htons((*event).port);
        (*event).udpip.address.sin_port = htons((*event).port + 1);

        (*event).tcpip.address.sin_addr.S_un.S_addr = INADDR_ANY;
        (*event).udpip.address.sin_addr.S_un.S_addr = INADDR_ANY;

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

        if (listen((*event).tcpip.socket, MAX_CONN) == SOCKET_ERROR)
        {
            printf("%s: listen TCP %s:%d failed. %s\n", __func__, "localhost", (*event).port, strerror(errno));
            return -1;
        }

        for (size_t i = 0; i < MAX_CONN; i++)
        {
            (*event).acc_list[i]._stat_ = stat_acc;
        }

        (*event).line = NULL;
        (*event).line_length = 0;

        return 0;
    }
    int app_event_free(acc_event *event)
    {
        close_socket(event->tcpip.socket);
        close_socket(event->udpip.socket);
        return 0;
    }
    void *_acc(void *arg)
    {
        acc_event *event = (acc_event *)arg;
        LOG("Server start at host:[http://localhost:%d]\n",event->port);
        while (1)
        {
            req_t request;
            int sizeof_req = sizeof(request.addr.address);

            __Accept__:
            request.addr.socket = accept(event->tcpip.socket, (sockaddr *)&request.addr.address, &sizeof_req);
            if (request.addr.socket == INVALID_SOCKET)
            {
                int err = get_error();
                if (err != 10035)
                {
                    LOG("%s accept Errorcode:%d %s\r\n", __func__, err, strerror(err));
                }
                goto __Accept__;
            }
            else
            {
                time(&(request.time));
                char buf[MAX_TIME_LEN];
                LOG("[%s](%s:%d)\r\n", getTMUTC(buf, MAX_TIME_LEN, event->UTCoffset, "%a %b %d %X %Y", &(request.time)), inet_ntoa(request.addr.address.sin_addr), request.addr.address.sin_port);
            START:
                for (size_t i = 0; i < MAX_CONN; i++)
                {
                    if ((*event).acc_list[i]._stat_ == stat_acc)
                    {
                        (*event).acc_list[i] = request;
                        (*event).acc_list[i]._stat_ = stat_rsc;
                        break;
                    }
                    else if (i == MAX_CONN - 1)
                    {
                        LOG("CONNECT MAX: %d", MAX_CONN);
                        send(request.addr.socket, "HTTP/1.1 503\r\n\0\0\0", 18, MSG_WAITALL);
                        _sleep(500);
                        goto START;
                    }
                }
            }
        }
        pthread_exit(NULL);
    }
    int app_acc(pthread_t *th, acc_event *event)
    {
        return pthread_create(th, NULL, _acc, event);
    }
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //!__APP_ACC__H__