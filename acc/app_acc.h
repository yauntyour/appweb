#pragma once
#ifndef __APP_ACC__H__
#define __APP_ACC__H__

#include "../com/com.h"
#include "../rsc/app_rsc.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

void sighandler(int signum)
{
    printf("signal code:%d,over\n", signum);
    exit(signum);
}
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
        if (bind((*event).tcpip.socket, (const struct sockaddr *)&(*event).tcpip.address, sizeof((*event).tcpip.address)) == SOCKET_ERROR)
        {
            printf("%s: bind TCP %s:%d failed. %s\n", __func__, "localhost", (*event).port, strerror(errno));
            return -1;
        }
        if (bind((*event).udpip.socket, (const struct sockaddr *)&(*event).udpip.address, sizeof((*event).udpip.address)) == SOCKET_ERROR)
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

    void *tcp_acc(void *arg)
    {
        appev_t *event = (appev_t *)arg;
        LOG_LIGHT("Server start at host:[http://localhost:%d]\n", event->port);
        size_t t = 0;
        while (1)
        {
            metadata_type(req_t *) request = (metadata_type(req_t *))metadata_alloc(sizeof(req_t), request_destroy);
#ifdef _WIN32
            int sizeof_req = sizeof((*(metadata_ptr(request))).addr.address);
#else
        socklen_t sizeof_req = sizeof((*(metadata_ptr(request))).addr.address);
#endif
        __Accept__:
            (*metadata_ptr(request)).addr.socket = accept(event->tcpip.socket, (struct sockaddr *)&(*metadata_ptr(request)).addr.address, &sizeof_req);
            if ((*metadata_ptr(request)).addr.socket == INVALID_SOCKET)
            {
                int err = get_error();
#ifdef _WIN32
                if (err != 10035)
                {
                    LOG_ERR("%s accept Errorcode:%d %s\r\n", __func__, err, strerror(err));
                }
#else
            if (err != EAGAIN)
            {
                LOG_ERR("%s accept Errorcode:%d %s\r\n", __func__, err, strerror(err));
            }
#endif
                goto __Accept__;
            }
            else
            {
                time(&((*metadata_ptr(request)).time));
                char buf[MAX_TIME_LEN];
                LOG_LIGHT("[%s](%s:%d)\r\n", getTMUTC(buf, MAX_TIME_LEN, event->UTCoffset, "%a %b %d %X %Y", &((*metadata_ptr(request)).time)), inet_ntoa((*metadata_ptr(request)).addr.address.sin_addr), (*metadata_ptr(request)).addr.address.sin_port);
            __Restart__:
                if (t >= event->MAXCONNECT)
                {
                    LOG_WARN("CONNECT MAX: %d\n", event->MAXCONNECT);
                    send((*metadata_ptr(request)).addr.socket, "HTTP/1.1 503\r\n", 15, MSG_WAITALL);
                    goto __Restart__;
                }
                else
                {
                    app_rsc(&t, request, event, TCP_CONNECT);
                    t += 1;
                }
            }
        }
        pthread_exit(NULL);
    }

    void *udp_acc(void *arg)
    {
        appev_t *event = (appev_t *)arg;
        LOG_LIGHT("Server start at host:[http://localhost:%d]\n", event->port);
        size_t t = 0;
        while (1)
        {
            metadata_type(req_t *) request = (metadata_type(req_t *))metadata_alloc(sizeof(req_t), request_destroy);
#ifdef _WIN32
            int sizeof_req = sizeof((*(metadata_ptr(request))).addr.address);
#else
        socklen_t sizeof_req = sizeof((*(metadata_ptr(request))).addr.address);
#endif
        __Accept__:
            int recvLen = recvfrom(event->udpip.socket, NULL, 0, 0,
                                   (struct sockaddr *)&(*metadata_ptr(request)).addr.address, &sizeof_req);
            if (recvLen <= 0)
            {
                int err = get_error();
#ifdef _WIN32
                if (err != 10035)
                {
                    LOG_ERR("%s accept Errorcode:%d %s\r\n", __func__, err, strerror(err));
                }
#else
            if (err != EAGAIN)
            {
                LOG_ERR("%s accept Errorcode:%d %s\r\n", __func__, err, strerror(err));
            }
#endif
                goto __Accept__;
            }
            else
            {
                time(&((*metadata_ptr(request)).time));
                char buf[MAX_TIME_LEN];
                LOG_LIGHT("[%s](%s:%d)\r\n", getTMUTC(buf, MAX_TIME_LEN, event->UTCoffset, "%a %b %d %X %Y", &((*metadata_ptr(request)).time)), inet_ntoa((*metadata_ptr(request)).addr.address.sin_addr), (*metadata_ptr(request)).addr.address.sin_port);
            __Restart__:
                if (t >= event->MAXCONNECT)
                {
                    LOG_WARN("CONNECT MAX: %d\n", event->MAXCONNECT);
                    sendto((*metadata_ptr(request)).addr.socket, "HTTP/1.1 503\r\n", 15, MSG_WAITALL,
                           (struct sockaddr *)&(*metadata_ptr(request)).addr.address, sizeof_req);
                    goto __Restart__;
                }
                else
                {
                    app_rsc(&t, request, event, UDP_CONNECT);
                    t += 1;
                }
            }
        }
        pthread_exit(NULL);
    }

    int app_acc(pthread_t *th, appev_t *event)
    {
        if (event->connect_type == TCP_CONNECT)
        {
            pthread_create(th, NULL, tcp_acc, event);
        }
        if (event->connect_type == UDP_CONNECT)
        {
            LOG_WARN("UDP HTTP is HTTP/3.0 may not support now,ensure your web browser is supported. version: %s\n", __SERVER__);
            pthread_create(th, NULL, udp_acc, event);
        }
        return 0;
    }
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //!__APP_ACC__H__