#ifndef __MEMC__H__
#define __MEMC__H__
#include <stdlib.h>
#include <pthread.h>
#ifdef __cplusplus
extern "C"
{
#endif
    void *_free(void *ptr)
    {
        if (ptr != NULL)
        {
            free(ptr);
            printf("%s:[%p] - %s \r\n",__func__, ptr, (char *)ptr);
            ptr = NULL;
        }
        else
        {
            fprintf(stdout, "\033[31m"
                            "memory free error at %s() ptr is NULL\r\n"
                            "\033[0m",
                    __func__);
        }
        pthread_exit(NULL);
    }
    void mem_free(void *p)
    {
        pthread_t thread;
        if (pthread_create(&thread, NULL, _free, p))
        {
            LOG_ERR("Thread[PID:%d] created fail at %s()\r\n", thread, __func__);
        }
        else
        {
            LOG_INFO("Thread[PID:%d] created at %s()\r\n", thread, __func__);
        }
        pthread_detach(thread);
    }
#ifdef __cplusplus
}
#endif
#endif //!__MEMC__H__