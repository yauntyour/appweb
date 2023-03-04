
# appweb (v3.0)

具备简单的集成性基础功能。
<!--more-->
模块架构：

- acc（接受请求）
- rsc（响应+调度器实现）
- RESRC（资源池）https://github.com/yauntyour/RESRC
- com（中心封装和集成）

全部二次封装&集成&C++ class封装于`appweb.h`。

# 版本更新内容

[updata.md](https://github.com/yauntyour/appweb/blob/master/updata_version.md)

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
    return "{'test':'Hello,World'}";
};

FUNC_CB_C(POST_TEST)
{
#ifdef _WIN32
    RESRC_FILE *p = RESRC_select_path(&res, "K:\\CCXXProgram\\appweb\\out\\post_test.html");
#else
    RESRC_FILE *p = RESRC_select_path(&res, "/workspaces/appweb/out/post_test.html");
#endif
    return p->data.data;
};

FUNC_CB_C(test)
{
    return "<h1 style='text-align:center;'>Hello,World</h1>";
};

FUNC_CB_C(img)
{
    // Inside the func to use send() need use FUNC_CB_HTML_OUT because need a "\r\n" character to terminate the header
    // 在func内部使用send（）需要使用func_CB_OUT，因为需要一个“\r\n”字符来终止标头
#ifdef _WIN32
    RESRC_FILE *p = RESRC_select_path(&res, "K:\\CCXXProgram\\appweb\\out\\image.jpg");
#else
    RESRC_FILE *p = RESRC_select_path(&res, "/workspaces/appweb/out/image.jpg");
#endif
    FUNC_CB_OUT(req->addr.socket, p->data.data, p->data.length, 0);
    // return a string containing the result of the '\0' is the first of character
    // 返回包含“\0”是第一个字符的结果的字符串
    return "";
}

#ifdef _WIN32
static FILE_PATH file_list[] = {
    {"K:\\CCXXProgram\\appweb\\out\\post_test.html", "rb"},
    {"K:\\CCXXProgram\\appweb\\out\\image.jpg", "rb"},
    {NULL, NULL}};
#else
static FILE_PATH file_list[] = {
    {"/workspaces/appweb/out/post_test.html", "rb"},
    {"/workspaces/appweb/out/image.jpg", "rb"},
    {NULL, NULL}};
#endif //_WIN32

int main(int argc, char const *argv[])
{
    LOG_LIGHT_NF("%s\n","Build application by Yauntyour (https://github.com/yauntyour) with C++");
    RESRC_create(&res, 2);
    for (size_t i = 0;file_list[i].path != NULL; i++)
    {
        // open the file ptr
        RESRC_FILE_OPEN(&(res.uuid_seed), &(res.filelist[i]), file_list[i].path, file_list[i].mode);
        // load the file data
        RESRC_FILE_cache(&(res.filelist[i]));
        LOG_LIGHT_NF("[RESRC::Load_file](path::%s,mode::%s)\n", file_list[i].path, file_list[i].mode);
    }
#ifdef _WIN32
    WS_Init();
#endif
    /*
    //used in C:
    appev_t ev;
    ev.port = 10000;
    ev.UTCoffset = 8;
    app_event_init(&ev, 128);

    ev.root_dict.func = test;
    ev.root_dict.req_Type = Type_ALL;
    ev.root_dict.resp_mime_type = "text/html";
    ev.root_dict.Name = NULL;
    ev.root_dict.ComPath = ComPath_False;
    */
    appweb app(8, 10000, 3);
    app.set_root_dict_func(test, Type_ALL, "text/html");

    // used in C:
    Varde home_list[] = {
        Varde_def(POST_TEST, Type_GET, "postTest", ComPath_True, "text/html"),
        Varde_def(api, Type_POST, "api", ComPath_True, "application/json"),
        Varde_def(img, Type_GET, "img", ComPath_True, "image/png"),
    };
    Varde home_dict = {test, Type_GET, "home", home_list, 3, 3, ComPath_True, "text/html"};
    /*
    //used in C:
    Varde_list_append(&(ev.root_dict), &home_dict);
    Varde_ZIP(&(ev.root_dict));
    Varde_ZIP(&home_dict);
    */
    home_dict.ZIP();
    app.root_dict_p->append(&home_dict);
    app.root_dict_p->ZIP();

    /*
    //used in C:
    pthread_t acc_th;
    app_acc(&acc_th, &ev);
    pthread_join(acc_th, NULL);
    */
    app.start(flag_wait);
#ifdef _WIN32
    WS_clean();
#endif
    for (size_t i = 0; file_list[i].path == NULL; i++)
    {
        RESRC_free(&(res.filelist[i]));
        RESRC_FILE_CLOSE(&(res.filelist[i]));
    }
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
   typedef char *(*func_cb)(req_t *, bytes *);
   //利用该宏可以快速定义一个标准函数（参见example dome）
   #define FUNC_CB_C(__name__) char *__name__(req_t *req, bytes *header)
   //显然我们可以通过bytes组件库提供的操作函数为header添加内容。
   //但是要注意，header尚未调用bytes_create。
   //理论上根据规范，您只需要返回页面的内容，调度器会自动写入响应流的末尾。
   
   //如果你要在定义的FUNC_CB里使用内容输出：请使用这个宏
   #define FUNC_CB_OUT(__arg__...) send(req->addr.socket, "\r\n",2, 0);send(__arg__) 
   //就像这样：
   FUNC_CB_C(img)
   {
       // Inside the func to use send() need use FUNC_CB_HTML_OUT because need a "\r\n" character to terminate the header
       // 在func内部使用send（）需要使用func_CB_OUT，因为需要一个“\r\n”字符来终止标头
       RESRC_FILE *p = RESRC_select_path(&res, "K:\\CCXXProgram\\appweb\\out\\bg.jpg");
       FUNC_CB_OUT(req->addr.socket, p->data.data, p->data.length, 0);
       // return a string containing the result of the '\0' is the first of character
       // 返回包含“\0”是第一个字符的结果的字符串
       return "";
   }
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

# 关键的依赖库

## Windows下使用appweb库看起来这样（socket 需要 wsock32.lib）

```
g++ -g XXXX.cpp -o XXXX -fexec-charset=UTF-8 -lwsock32 -lpthread
```

## Linux下使用appweb库看起来是这样的

```
g++ -g XXXX.cpp -o XXXX -fexec-charset=UTF-8 -lpthread
```

## 需要包含根目录下的appweb.h（C/C++完全封装）

# 后续更新信息

1. 加入zlib支持gzip压缩
2. 改进RESRC资源池设计
3. 支持HTTP2-3
4. UDP接口开放

### Made by yauntyour Copyright reserved.

### e-mail:yauntyour@outlook.com

### Copyright  see the file LICENSE.
