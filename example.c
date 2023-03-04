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
    RESRC_FILE *p = RESRC_select_path(&res, "appweb\\out\\post_test.html");
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
    RESRC_FILE *p = RESRC_select_path(&res, "appweb\\out\\bg.jpg");
#else
    RESRC_FILE *p = RESRC_select_path(&res, "/workspaces/appweb/out/image.jpg");
#endif
    FUNC_CB_OUT(req->addr.socket, p->data.data, p->data.length, 0);
    // return a string containing the result of the '\0' is the first of character
    // 返回包含“\0”是第一个字符的结果的字符串
    return "";
}

int main(int argc, char const *argv[])
{

    RESRC_create(&res, 2);

    // open the file ptr
#ifdef _WIN32
    RESRC_FILE_OPEN(&(res.uuid_seed), &(res.filelist[0]), "appweb\\out\\post_test.html", "rb");
    RESRC_FILE_OPEN(&(res.uuid_seed), &(res.filelist[1]), "appweb\\out\\bg.jpg", "rb");
#else
    RESRC_FILE_OPEN(&(res.uuid_seed), &(res.filelist[0]), "/workspaces/appweb/out/post_test.html", "rb");
    RESRC_FILE_OPEN(&(res.uuid_seed), &(res.filelist[1]), "/workspaces/appweb/out/image.jpg", "rb");
#endif
    // load the file data
    RESRC_FILE_cache(10, &(res.filelist[0]));
    RESRC_FILE_cache(10, &(res.filelist[1]));
#ifdef _WIN32
    WS_Init();
#endif
    
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
    /*
    appweb app(8, 10000, 3);
    app.set_root_dict_func(test, Type_ALL, "text/html");
    */
    // used in C:
    Varde home_list[] = {
        Varde_def(POST_TEST, Type_GET, "postTest", ComPath_True, "text/html"),
        Varde_def(api, Type_POST, "api", ComPath_True, "application/json"),
        Varde_def(img, Type_GET, "img", ComPath_True, "image/png"),
    };
    Varde home_dict = {test, Type_GET, "home", home_list, 3, 3, ComPath_True, "text/html"};
    
    //used in C:
    Varde_list_append(&(ev.root_dict), &home_dict);
    Varde_ZIP(&(ev.root_dict));
    Varde_ZIP(&home_dict);
    /*
    home_dict.ZIP();
    app.root_dict_p->append(&home_dict);
    app.root_dict_p->ZIP();
    */
    
    //used in C:
    pthread_t acc_th;
    app_acc(&acc_th, &ev);
    pthread_join(acc_th, NULL);
    /*
    app.start(flag_wait);
    */
#ifdef _WIN32
    WS_clean();
#endif
    RESRC_FILE_CLOSE(&(res.filelist[0]));
    return 0;
}