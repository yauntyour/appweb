#ifndef __STRUNIT__H__
#define __STRUNIT__H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    size_t find_str(char str[],size_t str_len, const char substr[])
    {
        size_t count = 0, i, j, check;
        size_t sublen = strlen(substr);
        for (i = 0; i < str_len; i++)
        {
            check = 1;
            for (j = 0; j + i < str_len && j < sublen; j++)
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

    size_t split(char *_Src, size_t _Src_len, const char *spt, char **strv[])
    {
        /*t is the temp offset*/
        size_t l = strlen(spt), c = 0, t = 0;

        (*strv) = (char **)calloc(find_str(_Src,_Src_len, spt), sizeof(char *));
        for (size_t i = 0; i < _Src_len;)
        {
            size_t j = 0;
        start:
            if (_Src[i + j] == spt[j])
            {
                if (j == l - 1)
                {
                    (*strv)[c] = (char *)calloc(i - t + 1, sizeof(char));
                    for (size_t x = 0; x < i - t; x++)
                    {
                        (*strv)[c][x] = _Src[t + x];
                    }
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
        (*strv)[c] = (char*)calloc(_Src_len - t + 1, sizeof(char));
        for (size_t x = 0; x < _Src_len - t; x++)
        {
            (*strv)[c][x] = _Src[t + x];
        }
        return c;
    }
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //!__STRUNIT__H__