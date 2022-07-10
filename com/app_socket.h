#pragma once
#ifndef __APP_SOCKET__H__
#define __APP_SOCKET__H__
#ifdef _WIN32

#include "unistd.h"
#ifndef _WINDOWS_
#include <winsock2.h>
#include <ws2tcpip.h>
#endif  //!_WINDOWS_
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

typedef SOCKET socket_t;

#endif

#ifndef _WIN32 // not _WIN32

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in, inet_addr
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h> // close,shutdown, write, read
#include <string.h>

typedef int socket_t;
#define INVALID_SOCKET (~0)
#define SOCKET_ERROR -1

#endif

int close_socket(socket_t sock)
{
#ifdef _WIN32
    return closesocket(sock);
#else
    return close(sock);
#endif
}

int shutdown_socket(socket_t sock)
{
#ifdef _WIN32
    return shutdown(sock, SD_BOTH);
#else
    return shutdown(sock, SHUT_RDWR);
#endif
}

void set_nonblocking(socket_t sock, unsigned int nonblocking)
{
#ifdef _WIN32
#ifdef __cplusplus
    auto flags = nonblocking ? 1UL : 0UL;
    ioctlsocket(sock, FIONBIO, &flags);
#else
    if (nonblocking == 1)
    {
        unsigned long flags = 1;
        ioctlsocket(sock, FIONBIO, &flags);
    }
    else
    {
        unsigned long flags = 0;
        ioctlsocket(sock, FIONBIO, &flags);
    }
#endif //__cplusplus
#else
#ifdef __cplusplus
    auto flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL,
          nonblocking ? (flags | O_NONBLOCK) : (flags & (~O_NONBLOCK)));
#endif //__cplusplus
#endif
}

int get_error()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

#ifdef _WIN32

int WS_Init()
{
    WSADATA wsaData;
    WSAStartup(0x0002, &wsaData);
    return 0;
}
int WS_clean()
{
    WSACleanup();
    return 0;
}

#ifdef __cplusplus

class WSInit
{
public:
    WSInit()
    {
        WS_Init();
    }
    ~WSInit() { WSACleanup(); }
};
static WSInit wsinit_; // 全局变量，windows下程序执行时构造初始化，退出时析构

#endif //__cplusplus

#endif
#endif //!__APP_SOCKET__H__