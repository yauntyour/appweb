#include <iostream>
#include <stdio.h>

#include "app_socket.h"
#include "../res/app_res.h"
#include "../acc/app_acc.h"
#include "../rsc/mime_type.h"

#include <pthread.h>

/*return NULL to success*/
void func(acc_event *event, req *request){
    
};
/*singal function must return a SIG_DFL or SIG_IGN singal*/
int sgl(acc_event *event, req *request);

int main(int argc, char const *argv[])
{
    WS_Init();
/*
    acc_event ev;
    ev.port = 10000;
    pthread_t app_acc_th = app_acc(&ev);

    pthread_join(app_acc_th, NULL);
*/
    bytes buf;
    buf.data = "GET ****";
    buf.length = 9;
    printf("%d",req_type(buf));
    printf("%s\n", mime_type(".html"));
    WS_clean();
    return 0;
}