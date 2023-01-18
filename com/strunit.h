#ifndef __STRUNIT__H__
#define __STRUNIT__H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    //  arr-二维字符串数组的指针
    int arr_addend(char ***arr, size_t arr_len, char *str, size_t strlen)
    {
        char **temp = (char **)calloc(arr_len + 1, sizeof(char *));
        if (arr_len != 0)
        {
            for (size_t i = 0; i < arr_len; i++)
            {
                temp[i] = (*arr)[i];
                //free((*arr)[i]);
                (*arr)[i] = NULL;
            }
        }
        temp[arr_len] = (char *)calloc(strlen + 1, sizeof(char));
        for (size_t i = 0; i < strlen; i++)
        {
            temp[arr_len][i] += str[i];
        }
        temp[arr_len][strlen + 1] = '\0';
        (*arr) = temp;
        return 0;
    }
    size_t split(char *_Src, size_t _Src_len, const char *spt, char **strv[])
    {
        size_t l = strlen(spt), c = 0, t = 0;
        for (size_t i = 0; i < _Src_len;)
        {
            size_t j = 0;
        start:
            if (_Src[i + j] == spt[j])
            {
                if (j == l - 1)
                {
                    arr_addend(strv, c, _Src + t, i - t);
                    c += 1;
                    t = (i + l);
                }
                else
                {
                    j += 1;
                    goto start;
                }
            }
            else if (j != 0)
            {
                i += j;
            }
            i += 1;
        }
        arr_addend(strv, c, _Src + t, _Src_len - t);
        return c;
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
#endif //!__STRUNIT__H__