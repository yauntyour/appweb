#ifndef __APP_SOCKET__H__
#define __APP_SOCKET__H__
#ifdef _WIN32

#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

typedef SOCKET socket_t;

#endif

#ifndef _WIN32 // not _WIN32

#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in, inet_addr
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h> // close,shutdown, write, read
#include <string.h>

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

void set_nonblocking(socket_t sock, bool nonblocking)
{
#ifdef _WIN32
    auto flags = nonblocking ? 1UL : 0UL;
    ioctlsocket(sock, FIONBIO, &flags);
#else
    auto flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL,
          nonblocking ? (flags | O_NONBLOCK) : (flags & (~O_NONBLOCK)));
#endif
}

bool get_error()
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

#endif
#endif //!__APP_SOCKET__H__