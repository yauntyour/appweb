# appweb (v2.0)

具备简单的集成性基础功能。

模块架构：

- acc（接受请求）
- rsc（响应+调度器实现）
- RESRC（资源池）https://github.com/yauntyour/RESRC
- com（中心封装和集成）

全部二次封装&集成&C++ class封装于`appweb.h`。

## 详细内容

### example dome：

```c
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "appweb.h"

static RESRC res;

FUNC_CB_C(api)
{
    char *html = "HTTP/1.1 200 OK \r\n\r\n{'test':'Hello,World'}\r\n \0\0\0";
    send((*request).addr.socket, html, strlen(html), 0);
    close_socket((*request).addr.socket);
};
FUNC_CB_C(login)
{
    RESRC_FILE *p = RESRC_select_path(&res, "K:\\CCXXProgram\\appweb\\out\\data.html");
    send((*request).addr.socket, p->data.data, p->data.length, 0);
    close_socket((*request).addr.socket);
};
FUNC_CB_C(test)
{
    char *html = "HTTP/1.1 200 OK \r\n\r\n<h1 style='text-align:center;'>Hello,World</h1>\r\n \0\0\0";
    send((*request).addr.socket, html, strlen(html), 0);
    close_socket((*request).addr.socket);
};

int main(int argc, char const *argv[])
{

    RESRC_create(&res, 1);

    RESRC_FILE_OPEN(&(res.uuid_seed), &(res.filelist[0]), "K:\\CCXXProgram\\appweb\\out\\data.html", "rb");
    RESRC_FILE_cache(10, &(res.filelist[0]));

    WS_Init();
    /*
    appev_t ev;
    ev.port = 10000;
    ev.UTCoffset = 8;
    ev.root_dict.func = test;
    app_event_init(&ev, 128);
    */
    appweb app(8, 10000, 2);

    /*
    Varde home_dict = Varde_def(test, Type_GET, "home", ComPath_True);
    Varde home_list[] = {
        Varde_def(login, Type_GET, "login", ComPath_True),
        Varde_def(api, Type_POST, "api", ComPath_True),
    };
    Varde_list_append(&(ev.root_dict), &home_dict);
    Varde_ZIP(&(ev.root_dict));
    Varde_ZIP(&home_dict);
    */
    app.set_root_dict_func(test, Type_ALL);

    Varde home_list[] = {
        Varde_def(login, Type_GET, "postTest", ComPath_True),
        Varde_def(api, Type_POST, "api", ComPath_True),
    };
    Varde home_dict = {test, Type_GET, "home", home_list, 2, 2, ComPath_True};
    home_dict.ZIP();

    app.root_dict_p->append(&home_dict);
    app.root_dict_p->ZIP();

    /*
     pthread_t acc_th;
     app_acc(&acc_th, &ev);
     pthread_t rsc_th;
     app_rsc(&rsc_th, &ev);
     pthread_join(rsc_th, NULL);
     pthread_join(acc_th, NULL);
 */
    app.start(flag_wait);

    WS_clean();
    RESRC_FILE_CLOSE(&(res.filelist[0]));
    return 0;
}
```

### 基本使用：
 Varde 的基本原理和结构————树级目录
[![IMG-20220827-123138.jpg](https://i.postimg.cc/X7HwZKgn/IMG-20220827-123138.jpg)](https://postimg.cc/ykRJtZ5G)

1. 注册一个Varde，其结构为

   ```C
    /*
        necessary arg:
            char* Name -A node's Name;
            func_cb func -A deal by function;
            int req_Type -request model;
            struct varde *list -A node list this node;
            size_t list_length, list_size -The list's length & size;
            int ComPath -Common path resolution:true or false;
        if req_model == -1,it will disable this Varde;
    */
    typedef struct varde
    {
        func_cb func;
        int req_Type;
        char *Name; // The Name of varde
        struct varde *list;
        size_t list_length, list_size;
        int ComPath; // Common path resolution:true or false
    #ifdef __cplusplus
        int append(struct varde *Var);
        int ZIP()
    #endif //__cplusplus
    } Varde;

   //函数类型为：
   //利用该宏可以快速定义一个标准函数（参见example dome）
    #define FUNC_CB_C(__name__) int __name__(req_t *request)
   //理论上根据规范，您需要返回一个HTTP状态码检查函数执行，因为在调度器中不会检查函数的执行是否正常，这就要您在编写函数时调试完成。
   ```
    我们提供一个快捷注册的宏
    ```c++
    #define Varde_def(func, req_Type, Name, ComPath)  \
        {                                             \
            func, req_Type, Name, NULL, 0, 0, ComPath \
        }
    //eg: Varde var = Varde_def(test,req_ALL,"test",ComPath_True);
    ```
2. 使用`app.start(flag_wait);`执行服务。执行端会监听您提供的port。flag设置为0表示默认阻塞运行。

### 相关基础信息：

```c++
typedef struct appev
{
    IPv4_addr_t tcpip, udpip;
    unsigned int port;
    Varde root_dict;
    req_t *Thread_queue;
    size_t Thread_queue_length;
    int UTCoffset;
} appev_t;
```

### SDK&Debug&Python实现的调试工具

1. nc.py

   ```python
   #参数
   if sys.argv[1] == '-nc':
       netcat()
   elif sys.argv[1] == '-cli':
       UDPclientSend()
   elif sys.argv[1] == '-server':
       Server()
   else:
       print("no this mode")
       print("argv format: -mode host port recvbufflen/data (Server socketTypes:STREAM/DGRAM)")
   ##argv format: -mode host port recvbufflen/data (Server socketTypes:STREAM/DGRAM)
   #eg :python nc.py -cli 127.0.0.1 10000 1280 STREAM
   ```

2. netools.py

   ```python
   #可以自行修改msg的内容
   msg = "GET /home HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Mozilla/5.0 (Windows NT 5.1; rv:10.0.2) Gecko/20100101 Firefox/10.0.2\r\n\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,/;q=0.8\r\nAccept-Language: zh-cn,zh;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\nReferer: http://localhost/\r\nContent-Length: 25\r\nContent-Type: application/x-www-form-urlencoded\r\nusername=aa&password=1234"
   
   if sys.argv[1] == '-nc':
       netcat()
   else:
       print("argv format: -mode host port recvbufflen")
   #argv format: -mode host port recvbufflen
   ```

3. netcat 自行到官网下载最新版本

   [netcat 1.11 for Win32/Win64 (eternallybored.org)](https://eternallybored.org/misc/netcat/)

# 后续更新信息

1. 加入zlib支持gzip压缩
2. 改进RESRC资源池设计
3. 支持HTTP2-3
4. UDP接口开放

### Made by yauntyour Copyright reserved.

### e-mail:yauntyour@outlook.com

### Copyright  see the file LICENSE.