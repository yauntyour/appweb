#ifndef __APP_RSC__H__
#define __APP_RSC__H__

#include "../com/app_socket.h"
#include "../acc/app_acc.h"
#include "../com/com.h"

#include <time.h>
#include <signal.h>
#include <pthread.h>

#ifdef _cplusplus
extern "C"
{
#endif //_cplusplus

    struct response_event
    {
        char header[128];
        time_t time;
        const char *content_type;
        char *url;
    };
    typedef response_event rsp;

    /*return NULL to success*/
    typedef void* (*func_t)(acc_event *event, req *request);

    /*singal function must return a SIG_DFL or SIG_IGN singal*/
    typedef int (*singal_t)(acc_event *event, req *request);

#define  SGL_DEF 1
#define  SGL_ING 0
    struct _onEvent_
    {
        struct _onEvent_ *next;
        singal_t sgl;
        func_t func;
    };
    typedef _onEvent_ onEvent;

    extern int app_on(acc_event *event, singal_t sgl, func_t func)
    {
        onEvent ev;
        ev.func = func;
        ev.sgl = sgl;
        ev.next = NULL;
        onEvent *eve = &((*event).event_list);
        while (eve->next)
        {
            eve = eve->next;
            if (eve->next == NULL)
            {
                eve->next = &ev;
            }
        }
    };
    struct __rsc_arg__
    {
        acc_event *event;
        req *request;
        onEvent *ev;
    };
    typedef __rsc_arg__ __rsc_arg__;
    extern void *__rsc__(void *arg)
    {
        __rsc_arg__ *ra = (__rsc_arg__*)arg;
        if ((*(*ra).ev).sgl((*ra).event,(*ra).request) == SGL_DEF)
        {
            return (*(*ra).ev).func((*ra).event,(*ra).request);
        }
    }
    extern int app_rsc(acc_event *event)
    {
        while (1)
        {
            for (size_t i = 0; i < MAX_CONN; i++)
            {
                if ((*event).acc_list[i]._stat_ == 1)
                {
                    onEvent *eve = &((*event).event_list);
                    while (eve->next)
                    {
                        __rsc_arg__ arg;
                        arg.event = event;
                        arg.request = &((*event).acc_list[i]);
                        arg.ev = eve;
                        pthread_t th;
                        if (pthread_create(&th, NULL, __rsc__, &arg) != 0)
                        {
                            return -1;
                        }
                        eve = eve->next;
                    }
                }
            }
        }
    }
#ifdef _cplusplus
}
#endif //_cplusplus
#endif //!__APP_RSC__H__