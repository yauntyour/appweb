#pragma once
#ifndef __ADDRESS__H__
#define __ADDRESS__H__
#include "app_socket.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

typedef struct IPv4_address{
    socket_t socket;
    sockaddr_in address;
} IPv4_addr_t;

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //!__ADDRESS__H__