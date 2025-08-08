#ifndef __APP_UDP__H__
#define __APP_UDP__H__
#include "../com/com.h"
#include "request.h"
#include "mime_type.h"

int app_udp(__search__arg_t *a, int *result, int *e)
{
    if (req_create(metadata_ptr(a->request)) == -1)
    {
        LOG_ERR("Failed to create requestHeader at %s() Result:%d\r\n", __func__, *result);
        rsp_404(metadata_ptr(a->request));
    }
    else
    {
        Varde *Temp = &(a->appev->root_dict);

        int addr_len = sizeof(metadata_ptr(a->request)->addr.address);

        // HTTP/1.1 200 OK\r\nDate: Thu Jan 26 10:57:27 2023\r\n
        metadata_type(bytes *) buf = (metadata_type(bytes *))metadata_alloc(sizeof(bytes), free);

        for (size_t i = 0; i < metadata_ptr(a->request)->url_slice_len; i++)
        {
            if (strlen(metadata_ptr(a->request)->url_slice[i]) == 0)
            {
                if (i == metadata_ptr(a->request)->url_slice_len - 1)
                {
                    if (a->appev->root_dict.func != NULL)
                    {
                        sendto(metadata_ptr(a->request)->addr.socket, "HTTP/1.1 200 OK\r\n", 17, 0,
                               (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        char time_buf[MAX_TIME_LEN];
                        getTMUTC(time_buf, MAX_TIME_LEN, a->appev->UTCoffset, "%a %b %d %X %Y", &(metadata_ptr(a->request)->time));
                        sendto(metadata_ptr(a->request)->addr.socket, time_buf, strlen(time_buf), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        sendto(metadata_ptr(a->request)->addr.socket, "\r\nContent-Type: ", 16, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        sendto(metadata_ptr(a->request)->addr.socket, Temp->resp_mime_type, strlen(Temp->resp_mime_type), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        sendto(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        char *html = a->appev->root_dict.func(metadata_ptr(a->request), metadata_ptr(buf));

                        if (metadata_ptr(buf)->data != NULL)
                        {
                            sendto(metadata_ptr(a->request)->addr.socket, metadata_ptr(buf)->data, strlen(metadata_ptr(buf)->data), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        }

                        if (html[0] != '\0')
                        {
                            sendto(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                            sendto(metadata_ptr(a->request)->addr.socket, html, strlen(html), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        }
                    }
                    else
                    {
                        goto __rsp__;
                    }
                }
                continue;
            }
            else
            {
                for (size_t j = 0; j < Temp->list_length; j++)
                {
                    if (strcmp(metadata_ptr(a->request)->url_slice[i], Temp->list[j].Name) == 0)
                    {
                        Temp = &(Temp->list[j]);
                        if (Temp->list == NULL && i < metadata_ptr(a->request)->url_slice_len - 1)
                        {
                            if (strlen(metadata_ptr(a->request)->url_slice[i + 1]) == 0)
                            {
                                goto __rsp__;
                            }
                            (*e) = 3;
                            goto __rsp_404__;
                        }
                        break;
                    }
                    else if (j == Temp->list_length - 1)
                    {
                        if (Temp->ComPath == COMPATH_True)
                        {
                            (*e) = 4;
                            goto __rsp__;
                        }
                        else
                        {
                            (*e) = 5;
                            goto __rsp_404__;
                        }
                    }
                }
                if (i == metadata_ptr(a->request)->url_slice_len - 1)
                {
                __rsp__:
                    if (Temp->req_Type == Type_ALL)
                    {
                        sendto(metadata_ptr(a->request)->addr.socket, "HTTP/1.1 200 OK\r\n", 17, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        char time_buf[MAX_TIME_LEN];
                        getTMUTC(time_buf, MAX_TIME_LEN, a->appev->UTCoffset, "%a %b %d %X %Y\r\n", &(metadata_ptr(a->request)->time));
                        sendto(metadata_ptr(a->request)->addr.socket, time_buf, strlen(time_buf), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        sendto(metadata_ptr(a->request)->addr.socket, "\r\nContent-Type: ", 16, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        sendto(metadata_ptr(a->request)->addr.socket, Temp->resp_mime_type, strlen(Temp->resp_mime_type), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        sendto(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        char *html = Temp->func(metadata_ptr(a->request), metadata_ptr(buf));

                        if (metadata_ptr(buf)->data != NULL)
                        {
                            sendto(metadata_ptr(a->request)->addr.socket, metadata_ptr(buf)->data, strlen(metadata_ptr(buf)->data), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        }

                        if (html[0] != '\0')
                        {
                            sendto(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                            sendto(metadata_ptr(a->request)->addr.socket, html, strlen(html), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        }

                        break;
                    }
                    else if (metadata_ptr(a->request)->req_model != NULL && strcmp(metadata_ptr(a->request)->req_model, Type_req[Temp->req_Type]) == 0)
                    {
                        sendto(metadata_ptr(a->request)->addr.socket, "HTTP/1.1 200 OK\r\nDate: ", 23, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        char time_buf[MAX_TIME_LEN];
                        getTMUTC(time_buf, MAX_TIME_LEN, a->appev->UTCoffset, "%a %b %d %X %Y", &(metadata_ptr(a->request)->time));
                        sendto(metadata_ptr(a->request)->addr.socket, time_buf, strlen(time_buf), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        sendto(metadata_ptr(a->request)->addr.socket, "\r\nContent-Type: ", 16, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        sendto(metadata_ptr(a->request)->addr.socket, Temp->resp_mime_type, strlen(Temp->resp_mime_type), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        sendto(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);

                        char *html = Temp->func(metadata_ptr(a->request), metadata_ptr(buf));

                        if (metadata_ptr(buf)->data != NULL)
                        {
                            sendto(metadata_ptr(a->request)->addr.socket, metadata_ptr(buf)->data, strlen(metadata_ptr(buf)->data), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        }

                        if (html[0] != '\0')
                        {
                            sendto(metadata_ptr(a->request)->addr.socket, "\r\n", 2, 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                            sendto(metadata_ptr(a->request)->addr.socket, html, strlen(html), 0, (struct sockaddr *)&metadata_ptr(a->request)->addr.address, addr_len);
                        }
                        break;
                    }
                    else
                    {
                    __rsp_404__:
                        rsp_404(metadata_ptr(a->request));

                        LOG_ERR("[Appweb::rsc::%s](msg::Response HTTP 404 Not Found. At %s()::errcode: %d)\r\n", __func__, __func__, e);
                        LOG_WARN_NF("[Appweb::rsc::%s](URL::", __func__);
                        for (size_t i = 0; i < metadata_ptr(a->request)->url_slice_len; i++)
                        {
                            if (i == metadata_ptr(a->request)->url_slice_len - 1)
                            {
                                LOG_WARN("/%s,mode::%s)\r\n", metadata_ptr(a->request)->url_slice[i], metadata_ptr(a->request)->req_model);
                            }
                            else
                            {
                                LOG_WARN_NF("/%s", metadata_ptr(a->request)->url_slice[i]);
                            }
                        }
                        break;
                    }
                }
            }
        }
        bytes_delete(metadata_ptr(buf));
        metadata_free(buf);
    }
    return 0;
}

#endif //!__APP_UDP__H__