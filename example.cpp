#include <iostream>
#include <stdio.h>
#include <time.h>
#include "appweb.h"

FUNC_CB_C(api)
{
    char *html = "HTTP/1.1 200 OK \r\n\r\n{'test':'Hello,World'}\r\n \0\0\0";
    send((*request).addr.socket, html, strlen(html), 0);
    close_socket((*request).addr.socket);
};
FUNC_CB_C(login)
{
    FILE *p = fopen("K:\\CCXXProgram\\appweb\\out\\data.html", "rb");
    size_t l = FIO_TELL(p);
    char *html = (char *)calloc(l, sizeof(char));
    fread(html, sizeof(char), l, p);
    send((*request).addr.socket, html, strlen(html), 0);
    free(html);
    close_socket((*request).addr.socket);
};
FUNC_CB_C(test)
{
    char *html = "HTTP/1.1 200 OK \r\n\r\n<h1>Hello,World</h1>\r\n \0\0\0";
    send((*request).addr.socket, html, strlen(html), 0);
    close_socket((*request).addr.socket);
};

int main(int argc, char const *argv[])
{

    WS_Init();
    /*
    appev_t ev;
    ev.port = 10000;
    ev.UTCoffset = 8;
    ev.root_dict.func = test;
    app_event_init(&ev, 128);
    */
    appweb app(8, 10000, 128);

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
        Varde_def(login, Type_GET, "login", ComPath_True),
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
    return 0;
}