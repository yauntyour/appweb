#ifndef __BYTES__H__
#define __BYTES__H__
#include <stdlib.h>
#include <string.h>

#ifdef _cplusplus
extern "C"
{
#endif //_cplusplus
    typedef char byter;
    typedef struct bytes
    {
        byter *data;
        size_t length;
    } bytes;

    extern size_t bytes_create(bytes *buf, size_t size)
    {
        (*buf).data = (byter *)calloc(size, sizeof(byter));
        (*buf).length = size;
        memset((*buf).data, 0, size);
        return (*buf).length;
    }
#ifdef _cplusplus
}

#endif //_cplusplus
#endif //!__BYTES__H__