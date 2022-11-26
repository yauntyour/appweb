#include <stdio.h>
#include "com/bytes.h"

int main(int argc, char const *argv[])
{
    bytes buf;
    bytes_create(&buf, 100);

    char *str = "012345678910";
    char *ptr = "Hello, World";

    bytes_write(&buf, str, strlen(str), Bytes_Mode_app);
    printf("OFFSET:%d, %s\n", buf.offset, buf.data);
    bytes_write(&buf, ptr, strlen(ptr), Bytes_Mode_app);
    printf("OFFSET:%d, %s\n", buf.offset, buf.data);

    bytes_delete(&buf);
    return 0;
}
