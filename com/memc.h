#ifndef __MEMC__H__
#define __MEMC__H__
#include <stdlib.h>
#include <pthread.h>

void *_free(void *ptr)
{
    free(ptr);
    ptr = NULL;
    pthread_exit(NULL);
}
void mem_free(void *p)
{
    pthread_t thread;
    pthread_create(&thread, NULL, _free, p);
    pthread_join(thread,NULL);
}
#endif //!__MEMC__H__