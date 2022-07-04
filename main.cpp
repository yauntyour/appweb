#include <iostream>
#include <stdio.h>
#include <time.h>
#include "com/com.h"
#include <pthread.h>

FUNC_CB_C(func)
{
    printf((*request).data.data);
    send((*request).addr.socket, "HTTP/1.1 200 OK \r\n  hello,world\r\n\0\0\0", 33, 0);
    close_socket((*request).addr.socket);
};

int main(int argc, char const *argv[])
{
    
    WS_Init();
    acc_event ev;
    ev.port = 10000;
    ev.UTCoffset = 8;
    app_event_init(&ev);

    urlc_t urlc[] = {
        {"",func,-1}
    };
    app_on(&ev,urlc,1);
    
    

    WS_clean();
    /*
        bytes buf;
        char *text1 = "POST /index.php HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1; rv:10.0.2) Gecko/20100101 Firefox/10.0.2\r\n\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,/;q=0.8\r\nAccept-Language: zh-cn,zh;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\nReferer: http://localhost/\r\nContent-Length: 25\r\nContent-Type: application/x-www-form-urlencoded\r\nusername=aa&password=1234";
        char *text2 = "POST /index.php HTTP/1.1";
        bytes_create(&buf,25);
        memcpy(buf.data,text2,25);

        char* arr[128] = {0};
        printf("%d\n",split(buf.data," ",arr));

        for (size_t i = 0; i < 128; i++)
        {
            if(arr[i] != NULL) printf("%s\n",arr[i]);
        }
    */
    return 0;
}