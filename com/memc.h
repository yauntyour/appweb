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
            ptr = NULL;
        }
        pthread_exit(NULL);
    }
    void mem_free(void *p)
    {
        pthread_t thread;
        pthread_create(&thread, NULL, _free, p);
        pthread_detach(thread);
    }
#ifdef __cplusplus
}
#endif
#endif //!__MEMC__H__