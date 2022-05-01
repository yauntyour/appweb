#ifndef __BYTES__H__
#define __BYTES__H__
#include <stdlib.h>
#include <string.h>

#ifdef _cplusplus
extern "C"
{
#endif //_cplusplus
    typedef char byte;
    typedef struct bytes
    {
        byte *data;
        size_t length;
    } bytes;

    extern size_t bytes_create(bytes *buf, size_t size)
    {
        (*buf).data = (byte *)calloc(size, sizeof(byte));
        (*buf).length = size;
        memset((*buf).data, 0, size);
        return (*buf).length;
    }
#ifdef _cplusplus
}

#endif //_cplusplus
#endif //!__BYTES__H__