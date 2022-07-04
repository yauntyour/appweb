#pragma once
#ifndef __APP_RES__H__
#define __APP_RES__H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <signal.h>

#include "sm3.h"
#include "FIO/fio.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
#define END_FILESET -1

    typedef struct file_path
    {
        char *path;
        char *mode;
        unsigned int active;
    }filepath_t;
    typedef struct _file
    {
        Asfio fp; //异步文件指针
        bytes data;
        unsigned char hash[32]; //基于国密sm3的hash计算
        filepath_t file;
        pthread_rwlock_t _luck_;
        int ifend;
    }_file_;
    typedef _file_ *fileset;

    int fileset_create(fileset *set, size_t size)
    {
        *set = (fileset)calloc(size, sizeof(_file));
        (*set)[size - 1].ifend = END_FILESET;
        return 0;
    }

    int fileopenc(_file_ *f, char *filepath, char *mode, unsigned int active)
    {
        f->fp = Asfio_create(filepath, mode);
        if (f->fp.Stream == NULL)
        {
            return -1;
        }
        f->file.path = filepath;
        f->file.active = active;
        f->file.mode = mode;
        pthread_rwlock_init(&(f->_luck_), NULL);
        return 0;
    }

    int fileopen(_file_ *f, filepath_t path)
    {
        (*f).fp = Asfio_create(path.path, path.mode);
        if (f->fp.Stream == NULL)
        {
            return -1;
        }
        (*f).file.path = path.path;
        (*f).file.active = path.active;
        (*f).file.mode = path.mode;
        pthread_rwlock_init(&((*f)._luck_), NULL);
        return 0;
    }

    size_t fileset_length(fileset *filetree)
    {
        size_t i = 0;
        while (1)
        {
            if ((*filetree)[i].ifend == END_FILESET)
            {
                break;
            }
            i += 1;
        }
        return i + 1;
    }

/*活动性评价*/
#define __ALWAYS__ 2
#define __ACTIVE__ 1
#define __INACTIVE__ 0

    int res_in(fileset *filetree, filepath_t *file_list, size_t num)
    {
        fileset_create(filetree, num);
        size_t i = 0;
        while (1)
        {
            if (fileopen(&(*filetree[i]), file_list[i]) == -1)
            {
                return i + 1;
            }
            if ((*filetree)[i].ifend == END_FILESET)
            {
                break;
            }
            i += 1;
        }
        return 0;
    }
    int res_cache(fileset *filetree, size_t length)
    {
        size_t i = 0;
        while (1)
        {
            if ((*filetree)[i].file.active == 0)
            {
                continue;
            }
            Asfio_readcall(&((*filetree)[i].fp), &((*filetree)[i].data));
            if ((*filetree)[i].ifend == END_FILESET)
            {
                break;
            }
            i += 1;
        }
        return 0;
    }
    void *res_active(void *arg)
    {
        fileset *filetree = (fileset *)arg;
        while (1)
        {
            _sleep(5 * 60 * 1000);
            for (size_t i = 0; i < fileset_length(filetree); i++)
            {
                pthread_rwlock_wrlock(&(*filetree)[i]._luck_);
                if ((*filetree)[i].file.active != __ALWAYS__)
                {
                    free((*filetree)[i].data.data);
                }
                pthread_rwlock_unlock(&(*filetree)[i]._luck_);
            }
        }
        return NULL;
    }

    /*arg format :
        -first --path
        -second --hash
    */
    _file_ *res_search(fileset *set, ...)
    {
        va_list va;
        va_start(va, set);

        char *path = va_arg(va, char *);
        unsigned char *hash = va_arg(va, unsigned char*);

        size_t i = 0;
        while (1)
        {
            pthread_rwlock_rdlock(&(*set)[i]._luck_);
            if ((*set)[i].hash == hash || (*set)[i].file.path == path)
            {
                pthread_rwlock_unlock(&(*set)[i]._luck_);
                return &((*set)[i]);
            }
            if ((*set)[i].ifend == END_FILESET)
            {
                pthread_rwlock_unlock(&(*set)[i]._luck_);
                break;
            }
            pthread_rwlock_unlock(&(*set)[i]._luck_);
            i += 1;
        }
        return NULL;
        va_end(va);
    }
    int res_awake(_file_ *file)
    {
        return Asfio_readcall(&((*file).fp), &((*file).data));
    }

    struct __res_args__
    {
        fileset *filetree;
        file_path *file_list;
        size_t num;
    };
    typedef __res_args__ res_args;

    pthread_t app_res(fileset *filetree, file_path *file_list, size_t num)
    {
        size_t i = res_in(filetree, file_list, num);
        if (i != 0)
        {
            perror("app.res.in():error");
        }

        size_t c = res_cache(filetree, num);
        if (c != 0)
        {
            perror("app.res.in():error");
        }
        pthread_t app_res_active;
        if(pthread_create(&app_res_active, NULL, res_active, filetree) == 0){
            //pthread_join(app_res_active,NULL);
            return app_res_active;
        }else{
            return 0;
        }
    }

// end
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //!__APP_RES__H__