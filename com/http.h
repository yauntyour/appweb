#pragma once
#ifndef __HTTP__H__
#define __HTTP__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTTP_SERVER "Server: "__SERVER__

#define Content_type_text(scription) "text/" #scription
#define Content_type_image(scription) "image/" #scription
#define Content_type_application(scription) "application/" #scription

#define Content_type_text_ 1
#define Content_type_image_ 2
#define Content_type_application_ 0

static char *RSP_404_HTML = "<!DOCTYPE html>"
                   "<head>"
                   "<title>404</title>"
                   "</head>"
                   "<body>"
                   "<h1 style='text-align:center'>"
                   "HTTP 404 Not Found"
                   "</h1>"
                   "</body>";

enum TYPE_REQ
{
    Type_ALL = 0,
    Type_GET, //请求获取Request-URI所标识的资源
    Type_POST,    //在Request-URI所标识的资源后附加新的数据
    Type_HEAD,    //请求获取由Request-URI所标识的资源的响应消息报头
    Type_PUT,     //请求服务器存储一个资源，并用Request-URI作为其标识
    Type_DELETE,  //请求服务器删除Request-URI所标识的资源
    Type_TRACE,   //请求服务器回送收到的请求信息，主要用于测试或诊断
    Type_CONNECT, // HTTP/1.1协议中预留给能够将连接改为管道方式的代理服务器
    Type_OPTIONS, //请求查询服务器的性能，或者查询与资源相关的选项和需求
};
static char *Type_req[] = {
    "ALL",
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

#include <strunit.h>

#endif //!__HTTP__H__