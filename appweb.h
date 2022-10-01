#ifndef __APPWEB__H__
#define __APPWEB__H__

#include "com/com.h"

#ifndef __APP_ACC__H__
#include "acc/app_acc.h"
#endif //!__APP_ACC__H__

#ifndef __APP_RSC__H__
#include "rsc/app_rsc.h"
#endif //!__APP_RSC__H__

#ifndef __RESRC__H__
#include "RESRC/RESRC.h"
#endif  //__RESRC__H__

#define flag_wait 0

#ifdef __cplusplus

class appweb
{
private:
    appev_t ev;
    pthread_t acc, //响应线程
        rsc,       //调度线程
        res;       //预留的资源线程
public:
    Varde *root_dict_p;
    appweb(int UTCoffset, unsigned int port, size_t MAXCONN);
    ~appweb();

    int set_root_dict_func(func_cb func, int req_Type);
    int start(int flag); //此函数默认阻塞运行 flag = 0
};

appweb::appweb(int UTCoffset, unsigned int port, size_t MAXCONN)
{
#ifdef _WIN32
    WS_Init();
#endif
    root_dict_p = &(ev.root_dict);
    ev.port = port;
    ev.UTCoffset = UTCoffset;
    app_event_init(&ev, MAXCONN);
}

appweb::~appweb()
{
    app_event_free(&ev);
#ifdef _WIN32
    WS_clean();
#endif
}

int appweb::set_root_dict_func(func_cb func, int req_Type)
{
    ev.root_dict.func = func;
    ev.root_dict.req_Type = req_Type;
    return 0;
}
int appweb::start(int flag)
{
    int _r_ = app_acc(&acc, &ev);
    if (_r_ != 0)
    {
        printf("Error message: acc thread create fail. Error code:%d\n", _r_);
        return _r_;
    }
    if (flag == 0)
    {
        pthread_join(acc, NULL);
        pthread_join(rsc, NULL);
    }
    else
    {
        return 0;
    }
    return 0;
}

#endif //__cplusplus

#endif //!__APPWEB__H__