#ifndef __RESRC__H__
#define __RESRC__H__
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#include "sm3.h"
#include "FIO/Asfio.h"
#include "UUID.h"

#define NULL_STAT 0
#define ONLINE_STAT 1
#define UNDERLINE_STAT -1
#define FREE_STAT -2
#define CLOSE_STAT -3

#define ERROR_RESRC ~0
static pthread_t *Thread_queue;

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    typedef struct _FILE_PATH
    {
        char *path;
        char *mode;
    } FILE_PATH;

    /*File的内容*/
    typedef struct _RESRC_FILE
    {
        FILE_PATH path;
        unsigned char hash[32], UUID[32];
        bytes data;
        Asfio asp;
        int stat;
    } RESRC_FILE;

    typedef struct _RESRC
    {
        RESRC_FILE *filelist;
        size_t Number;
        UUID_t uuid_seed;
    } RESRC;
    size_t RESRC_create(RESRC *p, size_t _NumOfRESRC)
    {
        (*p).filelist = (RESRC_FILE *)calloc(_NumOfRESRC, sizeof(RESRC_FILE));
        if ((*p).filelist != NULL)
        {
            memset((*p).filelist, 0, _NumOfRESRC);
            (*p).Number = _NumOfRESRC;
            (*p).uuid_seed = 0;
            for (size_t i = 0; i < _NumOfRESRC; i++)
            {
                (*p).filelist[i].stat = NULL_STAT;
            }
            return _NumOfRESRC;
        }
        return -1;
    }

    /*file为指向对应RESRC_FILE的指针，并非数组*/
    int RESRC_FILE_OPEN(UUID_t *p, RESRC_FILE *file, char *filepath, char *mode)
    {
        file->asp = Asfio_create(filepath, mode);
        file->path.path = filepath;
        file->path.mode = mode;
        file->stat = NULL_STAT;
        UUID_create(file->UUID, 32, p);
        return 0;
    }
    int RESRC_FILE_CLOSE(RESRC_FILE *file)
    {
        Asfio_close(&file->asp, 0);
        file->stat = CLOSE_STAT;
        return 0;
    }

    int RESRC_FILE_cache(RESRC_FILE *file)
    {
        if (file->stat == FREE_STAT)
        {
            return FREE_STAT;
        }
        else
        {
            if (Asfio_readcall(&(file->asp), &(file->data)) != 0)
            {
                file->stat = ONLINE_STAT;
                return file->data.length;
            }
            else
            {
                return ERROR_RESRC;
            };
        }
        return ONLINE_STAT;
    }

    int RESRC_free(RESRC_FILE *file)
    {
        if (file->stat == FREE_STAT)
        {
            return FREE_STAT;
        }
        else
        {
            free(file->data.data);
            file->data.length = 0;
            file->stat = FREE_STAT;
            return FREE_STAT;
        }
    }

    int RESRC_load_filelist(RESRC *res, FILE_PATH file_list[], size_t size)
    {
        RESRC_create(res, size);
        for (size_t i = 0; i < size; i++)
        {
            // open the file ptr
            RESRC_FILE_OPEN(&(res->uuid_seed), &(res->filelist[i]), file_list[i].path, file_list[i].mode);
            // load the file data
            RESRC_FILE_cache(&(res->filelist[i]));
            LOG_INFO_NF("[RESRC::Load_file](path::%s,mode::%s)\n", file_list[i].path, file_list[i].mode);
        }
        return 0;
    }

    RESRC_FILE *RESRC_select_path(RESRC *resrc, char *path)
    {
        for (size_t i = 0; i < resrc->Number; i++)
        {
            if (strcmp(resrc->filelist[i].path.path, path) == 0)
            {
                return resrc->filelist + i;
            }
        }
        return NULL;
    }
    RESRC_FILE *RESRC_select_UUID(RESRC *resrc, unsigned char UUID[32])
    {
        for (size_t i = 0; i < resrc->Number; i++)
        {
            for (size_t i = 0; i < 32; i++)
            {
                if ((resrc->filelist[i].UUID[i] ^ UUID[i]))
                {
                    break;
                }
                else
                {
                    return resrc->filelist + i;
                }
            }
        }
        return NULL;
    }
    RESRC_FILE *RESRC_select_hash(RESRC *resrc, unsigned char hash[32])
    {
        for (size_t i = 0; i < resrc->Number; i++)
        {
            for (size_t i = 0; i < 32; i++)
            {
                if ((resrc->filelist[i].hash[i] ^ hash[i]))
                {
                    break;
                }
                else
                {
                    return resrc->filelist + i;
                }
            }
        }
        return NULL;
    }
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //!__RESRC__H__