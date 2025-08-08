#ifndef __VARDE__H__
#define __VARDE__H__
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "app_socket.h"
#include "bytes.h"
#include "http.h"
#include "address.h"
#include "log.h"

typedef struct request_event
{
    time_t time;
    IPv4_addr_t addr;
    bytes data;
    char **url_slice;
    size_t url_slice_len;

    char req_model[8];
} req_t;

void request_destroy(void *ptr)
{
    req_t *req = (req_t *)ptr;
    bytes_delete(&(req->data));
    for (size_t i = 0; i < req->url_slice_len; i++)
    {
        free(req->url_slice[i]);
    }
    free(req->url_slice);
    free(req);
}

/*Varde*/
typedef char *(*func_cb)(req_t *, bytes *);

#define FUNC_CB_C(__name__) char *__name__(req_t *req, bytes *header)
#define FUNC_CB_OUT(__arg__...)           \
    send(req->addr.socket, "\r\n", 2, 0); \
    send(__arg__)

#define COMPATH_True 1
#define COMPATH_False 0

#define Varde_def(func, req_Type, Name, ComPath, resp_mime_type) \
    {                                                            \
        func, req_Type, Name, NULL, 0, 0, ComPath, resp_mime_type}

#define Varde_list_def(func, req_Type, Name, list, len, ComPath, resp_mime_type) {func, req_Type, Name, list, len, len, ComPath, resp_mime_type}

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
       /*
        necessary arg:
            char* Name :node's Name;
            func_cb func :A deal by function;
            int req_Type :request model;
            struct varde *list :A node list this node;
            size_t list_length, list_size :The list's length & size;
            int ComPath :Common path resolution:true or false;
        if req_model == -1,it will disable this Varde;
       */
    typedef struct varde
    {
        func_cb func;
        int req_Type;
        char *Name; // The Name of varde
        struct varde *list;
        size_t list_length;
        size_t list_size;
        int ComPath; // Common path resolution:true or false
        char *resp_mime_type;
#ifdef __cplusplus
        int append(struct varde *Var)
        {
            if (list_length < list_size)
            {
                list[list_length] = *Var;
                list_length += 1;
                return 0;
            }
            else
            {
                size_t l = (list_size + 1) * 2;
                struct varde *temp = (struct varde *)calloc(l, sizeof(struct varde));
                memset(temp, 0, l);
                if (temp != NULL)
                {
                    for (size_t i = 0; i < list_size; i++)
                    {
                        temp[i] = list[i];
                    }
                    temp[list_size] = *Var;
                    if (list != NULL)
                    {
                        free(list);
                    }

                    free(list);
                    list = temp;
                    list_length += 1;
                    list_size = l;
                    return 0;
                }
                else
                {
                    return -1;
                }
            }
        };
        int ZIP()
        {
            struct varde *temp = (struct varde *)calloc(list_length, sizeof(struct varde));
            if (temp == NULL)
            {
                return -1;
            }
            for (size_t i = 0; i < list_length; i++)
            {
                temp[i] = list[i];
            }
            list = temp;

            list_size = list_length;
            return 0;
        };
        int list_append(varde *dict, size_t len)
        {
            if (list_length + len < list_size)
            {
                for (size_t i = 0; i < len; i++)
                {
                    list[list_length + i] = dict[i];
                }
                list_length += len;
                return 0;
            }
            else
            {
                size_t l = (list_size + len) * 2;
                varde *temp = (varde *)calloc(l, sizeof(varde));
                memset(temp, 0, l);
                if (temp != NULL)
                {
                    for (size_t i = 0; i < list_size; i++)
                    {
                        temp[i] = list[i];
                    }
                    for (size_t i = 0; i < len; i++)
                    {
                        temp[list_size + i] = dict[i];
                    }
                    if (list != NULL)
                    {
                        free(list);
                    }
                    list = temp;
                    list_length += len;
                    list_size = l;
                    return 0;
                }
                else
                {
                    return -1;
                }
            }
        };
#endif //__cplusplus
    } Varde;
    int Varde_append(Varde *dict, Varde *Var)
    {
        if ((*dict).list_length < (*dict).list_size)
        {
            (*dict).list[(*dict).list_length] = *Var;
            (*dict).list_length += 1;
            return 0;
        }
        else
        {
            size_t l = ((*dict).list_size + 1) * 2;
            Varde *temp = (Varde *)calloc(l, sizeof(Varde));
            memset(temp, 0, l);
            if (temp != NULL)
            {
                for (size_t i = 0; i < (*dict).list_size; i++)
                {
                    temp[i] = (*dict).list[i];
                }
                temp[(*dict).list_size] = *Var;
                if ((*dict).list != NULL)
                {
                    free((*dict).list);
                }
                (*dict).list = temp;
                (*dict).list_length += 1;
                (*dict).list_size = l;
                return 0;
            }
            else
            {
                return -1;
            }
        }
    };
    int Varde_ZIP(Varde *dict)
    {
        Varde *temp = (Varde *)calloc((*dict).list_length, sizeof(Varde));
        if (temp == NULL)
        {
            return -1;
        }
        for (size_t i = 0; i < (*dict).list_length; i++)
        {
            temp[i] = dict->list[i];
        }
        (*dict).list = temp;
        (*dict).list_size = (*dict).list_length;
        return 0;
    };
    int Varde_list_append(Varde *dict, Varde *list, size_t len)
    {
        if ((*dict).list_length + len < (*dict).list_size)
        {
            for (size_t i = 0; i < len; i++)
            {
                (*dict).list[(*dict).list_length + i] = list[i];
            }
            (*dict).list_length += len;
            return 0;
        }
        else
        {
            size_t l = ((*dict).list_size + len) * 2;
            Varde *temp = (Varde *)calloc(l, sizeof(Varde));
            memset(temp, 0, l);
            if (temp != NULL)
            {
                for (size_t i = 0; i < (*dict).list_size; i++)
                {
                    temp[i] = (*dict).list[i];
                }
                for (size_t i = 0; i < len; i++)
                {
                    temp[(*dict).list_size + i] = list[i];
                }
                if ((*dict).list != NULL)
                {
                    free((*dict).list);
                }
                (*dict).list = temp;
                (*dict).list_length += len;
                (*dict).list_size = l;
                return 0;
            }
            else
            {
                return -1;
            }
        }
    };
#ifdef __cplusplus
};
#endif //__cplusplus
#endif //!__VARDE__H__