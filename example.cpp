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

    appweb app(8, 10000, 3);

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