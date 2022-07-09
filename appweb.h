#ifndef __APPWEB__H__
#define __APPWEB__H__

#include "com/com.h"
#ifndef __APP_ACC__H__
#include "acc/app_acc.h"
#endif //!__APP_ACC__H__
#ifndef __APP_RSC__H__
#include "rsc/app_rsc.h"
#endif //!__APP_RES__H__
#ifndef __APP_RES__H__
#include "res/app_res.h"
#endif //!__APP_RES__H__

#define flag_wait 0

#ifdef __cplusplus
class appweb
{
private:
    acc_event ev;
    pthread_t acc, //响应线程
        rsc,       //调度线程
        res;       //预留的资源线程
public:
    appweb(int UTCoffset, unsigned int port);
    ~appweb();
    int on(urlc_t *urlc, size_t len);
    int start(int flag); //此函数默认阻塞运行 flag = 0
};
appweb::appweb(int UTCoffset, unsigned int port)
{
    WS_Init();
    ev.port = port;
    ev.UTCoffset = UTCoffset;
    app_event_init(&ev);
}
appweb::~appweb()
{
    app_event_free(&ev);
    WS_clean();
}
int appweb::on(urlc_t *urlc, size_t len)
{
    return app_on(&ev, urlc, len);
}
int appweb::start(int flag)
{
    int _r_ = app_acc(&acc, &ev);
    if (_r_ != 0)
    {
        printf("Error message: acc thread create fail. Error code:%d\n", _r_);
        return _r_;
    }
    _r_ = app_rsc(&rsc, &ev);
    if (_r_ != 0)
    {
        printf("Error message: rsc thread create fail. Error code:%d\n", _r_);
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