#pragma once
#ifndef __BYTES__H__
#define __BYTES__H__
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#define Bytes_Stream_END 0

#define Bytes_Mode_Wb 0x1
#define Bytes_Mode_app 0x2

    typedef char byter;
    typedef struct bytes
    {
        byter *data;
        size_t length;
        size_t offset;
    } bytes;

    extern size_t bytes_create(bytes *buf, size_t size)
    {
        (*buf).data = (byter *)calloc(size, sizeof(byter));
        (*buf).length = size;
        (*buf).offset = 0;
        memset((*buf).data, 0, size);
        return (*buf).length;
    }

    extern size_t bytes_clean(bytes *buf)
    {
        memset((*buf).data, 0, (*buf).length);
        (*buf).offset = 0;
        return (*buf).length;
    }

    extern int bytes_delete(bytes *buf)
    {
        free((*buf).data);
        (*buf).length = 0;
        (*buf).offset = 0;
        return 0;
    }

    extern int bytes_copy(bytes *buf, bytes *src)
    {
        if (buf == NULL || src == NULL)
        {
            return -1;
        }
        if ((*buf).length < (*src).length)
        {
            bytes_create(buf, (*src).length);
            memcpy((*buf).data, (*src).data, (*src).length);
            return 0;
        }
        if ((*buf).length >= (*src).length)
        {
            for (size_t i = 0; i < (*src).length; i++)
            {
                (*buf).data[i] = (*src).data[i];
            }
            return 0;
        }
    }
    extern size_t bytes_seek(bytes *buf, size_t offset)
    {
        if (buf == NULL || offset > (*buf).length)
        {
            return -1;
        }
        (*buf).offset = offset;
        return offset;
    }
    extern size_t bytes_write(bytes *buf, byter *data, size_t size, int flags)
    {
        if (buf == NULL || data == NULL)
        {
            return -1;
        }
        if (flags == Bytes_Mode_Wb)
        {
            bytes_clean(buf);
            buf->offset = Bytes_Stream_END;
            for (size_t i = 0; i < buf->length; i++)
            {
                buf->data[i] = data[i];
                if (i == size - 1)
                {
                    break;
                }
            }
        }
        else if (flags == Bytes_Mode_app)
        {
            for (size_t i = 0; i < size; i++)
            {
                buf->data[buf->offset + i] = data[i];
                if (buf->offset + i == buf->length - 1 || i == size - 1)
                {
                    (*buf).offset += i + 1;
                    break;
                }
            }
        }
        else
        {
            return -1;
        }
        return buf->offset;
    }
#ifdef __cplusplus
};

#endif //__cplusplus
#endif //!__BYTES__H__