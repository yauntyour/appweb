# appweb

具备简单的集成性基础功能。

模块架构：

- acc（接受请求）
- rsc（响应+调度器实现）
- res（已独立为全新的结构）
- com（中心封装和集成）

全部二次封装&集成&C++ class封装于`appweb.h`。

## 详细内容

### example dome：

```c
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "appweb.h"

FUNC_CB_C(func)
{
    char *html = "HTTP/1.1 200 OK \r\n\r\n<h1>Hello,World</h1>\r\n \0\0\0";
    send((*request).addr.socket, html, strlen(html), 0);
    close_socket((*request).addr.socket);
};
int main(int argc, char const *argv[])
{
    /*
    //等价的C代码
    WS_Init();
    appev_t ev;
    ev.port = 10000;
    ev.UTCoffset = 8;
    app_event_init(&ev);
    */
    appweb app(8, 10000);
    
    urlc_t urlc[] = {
        {"/home", func, Type_GET},
    };
   	
    /*
    app_on(&ev, urlc, 1);

    pthread_t acc, rsc;
    app_acc(&acc, &ev);
    app_rsc(&rsc, &ev);
    pthread_join(acc, NULL);
    pthread_join(rsc, NULL);

    app_event_free(&ev);
    WS_clean();
    */
	app.on(urlc,1);
    app.start(flag_wait);
    delete &app;
    
    return 0;
}
```

### 基本使用：

1. 注册一个urlc_t的注册函数。结构为

   ```C
   /*
   necessary arg:
    char* url -URL path with on();
    func_cb _func_ -deal by function;
    int req_model,
        rsp_code;
   if req_model == -1,
   */
   typedef struct urlc
   {
       char *url;//受注册的URL
       func_cb _func_;//处理函数
       int req_model;//注册函数的响应类型
   } urlc_t;
   //函数类型为：
   typedef int (*func_cb)(req_t *);
   //理论上根据规范，您需要返回一个HTTP状态码检查函数执行，因为在调度器中不会检查函数的执行是否正常，这就要您在编写函数时调试完成。
   ```

2. 使用`app.start(flag_wait);`执行服务。执行端会监听您提供的port。flag设置为0表示默认阻塞运行。

### 相关基础信息：

```c++
typedef struct appev_t
{
    IPv4_addr_t tcpip, udpip;//保存了服务器的两个socket和服务器地址
    unsigned int port;//端口号
    req_t acc_list[MAX_CONN];//默认的最大连接数，需要初始化。可以修改MAX_CONN改变默认最大连接数
    size_t line_length;//注册函数列表的长度
    urlc_t *line;//注册函数列表
    int UTCoffset;//执行端所在地区的UTC偏移量，默认为0。
} appev_t;
//利用该宏可以快速定义一个标准函数（参见example dome）
#define FUNC_CB_C(__name__) int __name__(req_t *request)
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
2. 加入升级版res，资源池

### Made by yauntyour Copyright reserved.

### e-mail:yauntyour@outlook.com

### Copyright  see the file LICENSE.