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
            (*request).reqline_len = split((*request).data.data, (*request).data.length, "\r\n", &(*request).reqline);
            if ((*request).reqline_len == 0)
                return -1;

            char **dest;
            if (split((*request).reqline[0], strlen((*request).reqline[0]), " ", &dest) == 0)
                return -1;
                
            (*request).req_model = dest[0];
            (*request).url_slice_len = split(dest[1], strlen(dest[1]), "/", &(*request).url_slice) + 1;
            if ((*request).url_slice_len == 0)
                return -1;
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
        mem_free(buf);
        close_socket((*request).addr.socket);
        return 404;
    }
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //!__REQUEST__H__