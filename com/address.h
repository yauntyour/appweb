#ifndef __ADDRESS__H__
#define __ADDRESS__H__
#include "app_socket.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _cplusplus
extern "C"
{
#endif //_cplusplus

typedef struct IPv4_address{
    socket_t socket;
    sockaddr_in address;
} IPv4_addr_t;

#ifdef _cplusplus
}
#endif //_cplusplus
#endif //!__ADDRESS__H__