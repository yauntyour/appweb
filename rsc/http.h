#ifndef __HTTP__H__
#define __HTTP__H__
#include "../com/com.h"

#define HTTP_SERVER "Server: "__SERVER__

#define Content_type_text(scription) "text/" #scription
#define Content_type_image(scription) "image/" #scription
#define Content_type_application(scription) "application/" #scription

#define Content_type_text_ 1
#define Content_type_image_ 2
#define Content_type_application_ 0

/*
    "text/html"
    "text/plain"
    "text/xml"
    "image/gif"
    "image/jpeg"
    "image/png"
*/
#endif //!__HTTP__H__