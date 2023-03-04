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
    LOG_LIGHT_NF("%s\n","Build application by Yauntyour (https://github.com/yauntyour) with C");
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

    // used in C:
    appev_t ev;
    ev.port = 10000;
    ev.UTCoffset = 8;
    app_event_init(&ev, 128);
    
    //root event
    ev.root_dict.func = test;
    ev.root_dict.req_Type = Type_ALL;
    ev.root_dict.resp_mime_type = "text/html";
    ev.root_dict.Name = NULL;
    ev.root_dict.ComPath = ComPath_False;
    //root list
    ev.root_dict.list_length = 0;
    ev.root_dict.list_size = 0;
    ev.root_dict.list = NULL;
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

    // used in C:
    Varde_list_append(&(ev.root_dict), &home_dict);
    Varde_ZIP(&(ev.root_dict));
    Varde_ZIP(&home_dict);
    /*
    home_dict.ZIP();
    app.root_dict_p->append(&home_dict);
    app.root_dict_p->ZIP();
    */

    // used in C:
    pthread_t acc_th;
    app_acc(&acc_th, &ev);
    pthread_join(acc_th, NULL);
    /*
    app.start(flag_wait);
    */
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