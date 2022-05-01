#ifndef __FIO__H__
#define __FIO__H__

extern size_t FIO_TELL(FILE*p){
    rewind(p);
    fseek(p,0L,SEEK_END);
    size_t i = ftell(p);
    rewind(p);
    return i;
}
#include "fiotypes.h"
#include "Asfio.h"

#endif  //!__FIO__H__