# v3.0更新内容

## v2.3:

- 完善strunit.h的实现模式
- 移除了氨醛的memc操作库
- 为appweb添加了全新的库——metadata，它最大的特定是可以为所申请的内存注册析构函数
- 改进了rsc的传输机制（使其形成真正意义上的Asyc_out）
- 结构化改进

## v3.0:

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
FUNC_CB_C(login)
{
    RESRC_FILE *p = RESRC_select_path(&res, "K:\\CCXXProgram\\appweb\\out\\data.html");
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
    RESRC_FILE *p = RESRC_select_path(&res, "K:\\CCXXProgram\\appweb\\out\\bg.jpg");
    FUNC_CB_OUT(req->addr.socket, p->data.data, p->data.length, 0);
    // return a string containing the result of the '\0' is the first of character
    // 返回包含“\0”是第一个字符的结果的字符串
    return "";
}

int main(int argc, char const *argv[])
{

    RESRC_create(&res, 2);

    // open the file ptr
    RESRC_FILE_OPEN(&(res.uuid_seed), &(res.filelist[0]), "K:\\CCXXProgram\\appweb\\out\\data.html", "rb");
    RESRC_FILE_OPEN(&(res.uuid_seed), &(res.filelist[1]), "K:\\CCXXProgram\\appweb\\out\\bg.jpg", "rb");
    // load the file data
    RESRC_FILE_cache(10, &(res.filelist[0]));
    RESRC_FILE_cache(10, &(res.filelist[1]));

    WS_Init();
    /*
    //used in C:
    appev_t ev;
    ev.port = 10000;
    ev.UTCoffset = 8;
    ev.root_dict.func = test;
    app_event_init(&ev, 128);
    */
    appweb app(8, 10000, 3);

    /*
    //used in C:
    Varde home_dict = Varde_def(test, Type_GET, "home", ComPath_True);
    Varde home_list[] = {
        Varde_def(login, Type_GET, "login", ComPath_True),
        Varde_def(api, Type_POST, "api", ComPath_True),
    };
    Varde_list_append(&(ev.root_dict), &home_dict);
    Varde_ZIP(&(ev.root_dict));
    Varde_ZIP(&home_dict);
    */

    app.set_root_dict_func(test, Type_ALL, "text/html");

    Varde home_list[] = {
        Varde_def(login, Type_GET, "postTest", ComPath_True, "text/html"),
        Varde_def(api, Type_POST, "api", ComPath_True, "application/json"),
        Varde_def(img, Type_GET, "img", ComPath_True, "image/png"),
    };
    Varde home_dict = {test, Type_GET, "home", home_list, 3, 3, ComPath_True, "text/html"};
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

    WS_clean();
    RESRC_FILE_CLOSE(&(res.filelist[0]));
    return 0;
}
```

Varde 上的更新:

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

注册宏的更新：

```c++
#define Varde_def(func, req_Type, Name, ComPath, resp_mime_type)  \
    {                                                             \
        func, req_Type, Name, NULL, 0, 0, ComPath, resp_mime_type \
    }
//eg: Varde var = Varde_def(test,req_ALL,"test",ComPath_True,);
```

## 对C的支持优化（暂不明白神秘bug如何产生的）

