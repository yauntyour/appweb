#ifndef __REQUEST__H__
#define __REQUEST__H__

#include "../com/com.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
    int req_create(req_t *request)
    {
        if (request->data.data != NULL)
        {
            split((*request).data.data, "\r\n", (*request).reqline);
            char *dest[3] = {0};
            split((*request).reqline[0], " ", dest);
            (*request).req_model = dest[0];
            (*request).url = dest[1];
            return 0;
        }
        return -1;
    }
    int rsp_404(req_t *request)
    {
        size_t len = 29 + strlen(RSP_404_HTML) + 1;
        char *buf = (char *)calloc(len, sizeof(char));
        sprintf(buf, "HTTP/1.1 404 Not Found\r\n\r\n%s", RSP_404_HTML);
        send((*request).addr.socket, buf, len, 0);
        free(buf);
        close_socket((*request).addr.socket);
        return 404;
    }
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //!__REQUEST__H__