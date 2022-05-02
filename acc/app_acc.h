#ifndef __APP_ACC__H__
#define __APP_ACC__H__
#include "../com/app_socket.h"
#include "../com/address.h"
#include "../com/bytes.h"
#include "../com/com.h"

#include "acc_def.h"

#include "../rsc/app_rsc.h"
#include "../res/app_res.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#ifdef _cplusplus
extern "C"
{
#endif //_cplusplus

    struct request_event
    {
        time_t time;
        IPv4_addr_t addr;
        bytes data;
        int req_type;
        int _stat_;
    };
    typedef request_event req;

    typedef struct acc_event
    {
        IPv4_addr_t tcpip, udpip;
        unsigned int port;
        req acc_list[MAX_CONN];
        onEvent event_list;
    } acc_event;

    extern int req_type(bytes data)
    {
        for (size_t i = 0; i < data.length; i++)
        {
            if (data.data[i] == ' ')
            {
                for (size_t j = 0; Type_req[j] != NULL; j++)
                {
                    if (strpbrk(data.data, Type_req[j]) != NULL)
                    {
                        return j;
                    }
                    else
                    {
                        printf("%s error request type ", __func__);
                        for (size_t j = 0; j < i; j++)
                        {
                            printf("%c", data.data[j]);
                        }
                        printf("\n");
                    }
                }
            }
        }
    }

    extern int event_init(acc_event *event)
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
        (*event).event_list.func = NULL;
        (*event).event_list.sgl = NULL;
        (*event).event_list.next = NULL;
        return 0;
    }

    extern void *event_acc(void *arg)
    {
        acc_event *event = (acc_event *)arg;
        while (1)
        {
            req request;
            request._stat_ = 1;
            bytes_create(request.data.data, MAX_RECV_BUF);
            int sizeof_req = sizeof(request.addr.address);
            request.addr.socket = accept(event->tcpip.socket, (sockaddr *)&request.addr.address, &sizeof_req);
            if (request.addr.socket == INVALID_SOCKET)
            {
                int err = get_error();
                printf("%s accept Errorcode:%d %s\r", __func__, err, strerror(err));
                _sleep(100);
            }
            else
            {
                recv(request.addr.socket, request.data.data, MAX_RECV_BUF, 0);

            START:
                for (size_t i = 0; i < MAX_CONN; i++)
                {
                    if ((*event).acc_list[i]._stat_ == stat_acc)
                    {
                        (*event).acc_list[i] = request;
                        (*event).acc_list[i]._stat_ = stat_rsc;
                        // send((*event).acc_list[i].addr.socket, "HTTP/1.1 200 OK \r\n  hello,world\r\n\0\0\0", 33, 0);
                        if (i == MAX_CONN - 1)
                        {
                            printf("CONNECT MAX: %d", MAX_CONN);
                        }
                        break;
                    }
                }
                _sleep(500);
                goto START;
            }
        }
        return NULL;
    }

    extern pthread_t app_acc(acc_event *event)
    {
        event_init(event);
        pthread_t acc_th;
        if (pthread_create(&acc_th, NULL, event_acc, event) == 0)
        {
            return acc_th;
        }
        else
        {
            return 0;
        }
    }

#ifdef _cplusplus
}
#endif //_cplusplus

#endif //!__APP_ACC__H__