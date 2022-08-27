#ifndef __ASFIO__H__
#define __ASFIO__H__
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "fiotypes.h"
#include "fio.h"
#include "bytes.h"

typedef struct Asfio // asynchronous I/O
{
    FILE *Stream;
    // luck with asyio
    pthread_rwlock_t AsfioLuck;
} Asfio;
// pthread_t redefine
typedef unsigned long long CALLBLACK_TH;

extern Asfio Asfio_create(const char *filepath, const char *mode)
{
    Asfio asfio;
    // create a luck
    pthread_rwlock_init(&asfio.AsfioLuck, NULL);
    // get file port.
    FILE *p = fopen(filepath, mode);
    if (p == NULL)
    {
        fprintf(stderr, "%s", FILEOPENERR);
        asfio.Stream = NULL;
        return asfio;
    }
    // get file stream
    asfio.Stream = p;
    return asfio;
}

// read function
void *__read__(void *args)
{
    Asfio *asp = (Asfio *)args;
    pthread_rwlock_rdlock(&asp->AsfioLuck);
    size_t i = FIO_TELL(asp->Stream);
    bytes filedata;
    bytes_create(&filedata, i);
    fseek(asp->Stream, 0L, SEEK_SET);
    if (fread(filedata.data, 1ULL, i, asp->Stream) == i)
    {
        rewind(asp->Stream);
        pthread_rwlock_unlock(&asp->AsfioLuck);
        return (void *)&filedata;
    }
    else
    {
        rewind(asp->Stream);
        pthread_rwlock_unlock(&asp->AsfioLuck);
        fprintf(stderr, "%s\n", "Asfio read has error");
        return (void *)NULL;
    }
}
extern CALLBLACK_TH Asfio_readall(Asfio *asp)
{
    if (asp != NULL)
    {
        pthread_t th;
        if (pthread_create(&th, NULL, __read__, (void *)asp) == 0)
        {
            return th;
        }
        else
        {
            fprintf(stderr, "%s\n", "Asfio thread create fail");
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "%s", FILEIOPOTERR);
        return 0;
    }
}
// callblack function
extern size_t Asfio_callblack(CALLBLACK_TH th, bytes *res)
{
    if (th != -1 & th != 0)
    {
        if (pthread_join(th, (void **)&res->data) == 0)
        {
            if (res->data != NULL)
            {
                return res->length;
            }
            else
            {
                fprintf(stderr, "%s\n", "Asfio Callblack function::return NULL");
                return 0;
            }
        }
        else
        {
            fprintf(stderr, "%s\n", " Asfio Callblack function::join() error.");
            return 0;
        }
    }
    else
    {
        fprintf(stderr, "%s\n", "Asfio Callblack function error.");
        return NULL;
    }
}

// define a args struct
typedef struct
{
    Asfio *asp;
    const void *Element;
    size_t size;
} W_args;
// write function
void *__write__(void *args)
{
    W_args *wargs = (W_args *)args;
    pthread_rwlock_wrlock(&wargs->asp->AsfioLuck);
    if (fwrite(wargs->Element, 1ULL, wargs->size, wargs->asp->Stream) == wargs->size)
    {
        pthread_rwlock_unlock(&wargs->asp->AsfioLuck);
        return (void *)0;
    }
    else
    {
        pthread_rwlock_unlock(&wargs->asp->AsfioLuck);
        fprintf(stderr, "%s\n", "Asfio writer has error");
        return (void *)-1;
    }
};

extern int Asfio_write(Asfio *asp, const void *Element, size_t size)
{
    if (asp != NULL)
    {
        W_args wargs;
        wargs.asp = asp;
        wargs.Element = Element;
        wargs.size = size;
        pthread_t th;
        if (pthread_create(&th, NULL, __write__, (void *)&wargs) == 0)
        {
            pthread_join(th, NULL);
            return 1;
        }
        else
        {
            fprintf(stderr, "%s\n", "Asfio thread create fail");
            return 0;
        }
    }
    else
    {
        fprintf(stderr, "%s%s", FILEIOPOTERR);
        return 0;
    }
}
// Asfio close function
extern int Asfio_close(Asfio *asp, int mode)
{
    pthread_rwlock_wrlock(&asp->AsfioLuck);
    pthread_rwlock_unlock(&asp->AsfioLuck);
    pthread_rwlock_destroy(&asp->AsfioLuck);
    if (mode == 0)
    {
        fclose(asp->Stream);
        return 1;
    }
    else if (mode == -1)
    {
        free(asp);
        Asfio new_asp;
        asp = &new_asp;
        return 1;
    }
    else
    {
        fprintf(stderr, "%s\n", "Asfio didn't have this mode.");
        return 0;
    }
}

// Asfio readcall() function
extern size_t Asfio_readcall(Asfio *asp, bytes *buf)
{
    if (asp != NULL)
    {
        pthread_rwlock_rdlock(&asp->AsfioLuck);
        size_t i = FIO_TELL(asp->Stream);
        bytes_create(buf, i);
        fseek(asp->Stream, 0L, SEEK_SET);
        if (fread(buf->data, 1ULL, i, asp->Stream) == i)
        {
            rewind(asp->Stream);
            pthread_rwlock_unlock(&asp->AsfioLuck);
        }
        else
        {
            rewind(asp->Stream);
            pthread_rwlock_unlock(&asp->AsfioLuck);
            fprintf(stderr, "%s\n", "Asfio read has error");
        }
        return buf->length;
    }
    else
    {
        fprintf(stderr, "%s", FILEIOPOTERR);
        return 0;
    }
}
#endif //!__ASFIO__H__