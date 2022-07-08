#pragma once
#ifndef __HTTP__H__
#define __HTTP__H__

#define HTTP_SERVER "Server: "__SERVER__

#define Content_type_text(scription) "text/" #scription
#define Content_type_image(scription) "image/" #scription
#define Content_type_application(scription) "application/" #scription

#define Content_type_text_ 1
#define Content_type_image_ 2
#define Content_type_application_ 0

static char *RSP_404_HTML = "{\"404\":\"404 Not Found!\"}";

enum TYPE_REQ
{
    Type_GET = 0, //请求获取Request-URI所标识的资源
    Type_POST,    //在Request-URI所标识的资源后附加新的数据
    Type_HEAD,    //请求获取由Request-URI所标识的资源的响应消息报头
    Type_PUT,     //请求服务器存储一个资源，并用Request-URI作为其标识
    Type_DELETE,  //请求服务器删除Request-URI所标识的资源
    Type_TRACE,   //请求服务器回送收到的请求信息，主要用于测试或诊断
    Type_CONNECT, // HTTP/1.1协议中预留给能够将连接改为管道方式的代理服务器
    Type_OPTIONS, //请求查询服务器的性能，或者查询与资源相关的选项和需求
};

static char *Type_req[] = {
    "GET",
    "POST",
    "HEAD",
    "PUT",
    "DELETE",
    "TRACE",
    "CONNECT",
    "OPTIONS",
    NULL,
};
/*Max length is*/
#define HTTP_response_line_MAX_len 128
#define Content_type_MAX_len 64
#define HTTP_response_line(status_code, description) "HTTP/1.1 " #status_code #description "\r\n"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    /*
            src 源字符串的首地址(buf的地址)
            separator 指定的分割字符
            dest 接收子字符串的数组
    */
    size_t split(char *src, const char *spt, char **dest)
    {

        char *pNext = NULL, *pTemp = NULL;
        size_t count = 0;
        if (src == NULL || strlen(src) == 0)
        {
            //如果传入的地址为空或长度为0，直接终止
            return -1;
        }
        if (spt == NULL || strlen(spt) == 0)
        {
            //如未指定分割的字符串，直接终止
            return -1;
        }
        pNext = strtok_r(src, spt, &pTemp);
        while (pTemp != NULL)
        {
            dest[count] = pNext;
            pNext = NULL;
            count += 1;
            pNext = strtok_r(NULL, spt, &pTemp);
        }
        return count;
    }

    size_t find_str(char str[], char substr[])
    {
        size_t count = 0, i, j, check;
        size_t len = strlen(str);
        size_t sublen = strlen(substr);
        for (i = 0; i < len; i++)
        {
            check = 1;
            for (j = 0; j + i < len && j < sublen; j++)
            {
                if (str[i + j] != substr[j])
                {
                    check = 0;
                    break;
                }
            }
            if (check == 1)
            {
                count++;
                i = i + sublen;
            }
        }

        return count;
    }
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //!__HTTP__H__