#ifndef __ACC_DEF__H__
#define __ACC_DEF__H__

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
}

#define HTTP_response_line(status_code, description) "HTTP/1.1 " #status_code #description "\r\n"

#endif //!__ACC_DEF__H__