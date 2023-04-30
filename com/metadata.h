#ifndef __METADATA__H__
#define __METADATA__H__
#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

    typedef struct __metadata__struct__
    {
        void *__ptr__;
        size_t __allocated__;
        size_t __count__;
        void (*__destruct__)(void *_ptr);
    } metadata;

    // allocate function
    void *__metadata_alloc__(size_t size, void (*__destruct__)(void *_ptr))
    {
        metadata *meta = (metadata *)malloc(sizeof(metadata));

        meta->__ptr__ = malloc(size);
        memset(meta->__ptr__, 0, size);

        meta->__count__ = 0;
        assert(__destruct__ != NULL);
        meta->__destruct__ = __destruct__;
        meta->__allocated__ = size;
        return meta;
    }
    void __metadata_free__(void *meta)
    {
        metadata *_meta = (metadata *)meta;

        assert(meta != _meta->__ptr__);
        assert(_meta->__count__ == 0);
        _meta->__destruct__(_meta->__ptr__);
        _meta->__ptr__ = NULL;
        free(*(void **)meta);
        meta = NULL;
    }
#define metadata_alloc(size, destruct) (__metadata_alloc__(size, destruct))
#define metadata_free(meta_ptr) (__metadata_free__(meta_ptr))
#define metadata_ptr(meta_ptr) (*meta_ptr)

#define metadata_quote(meta_ptr) ((metadata*)meta_ptr)->__count__ += 1
#define metadata_unquote(meta_ptr) ((metadata*)meta_ptr)->__count__ -= 1

#define metadata_type(Typename) Typename *
#define metadata_dtorc(funcName) void funcName(void *ptr)

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //!__METADATA__H__