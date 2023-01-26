#ifndef __REQUEST__H__
#define __REQUEST__H__

#include "../com/com.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
    int req_create(req_t *request)
    {
        size_t req_len = strlen((*request).data.data);
        if (request->data.data != NULL && req_len != 0)
        {
            size_t i = 0;
            while (1)
            {
                if (request->data.data[i] == ' ')
                {
                    request->req_model[i] = '\0';
                    i += 1;
                    break;
                }
                request->req_model[i] = request->data.data[i];
                i += 1;
            }
            size_t j = i;
            while (1)
            {
                if (request->data.data[i] == ' ')
                {
                    break;
                }
                i += 1;
            }
            request->url_slice_len = split(request->data.data + j, i - j, "/", &(request->url_slice)) + 1;
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