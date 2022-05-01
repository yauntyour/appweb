#include <iostream>
#include <stdio.h>

//#include "app_socket.h"
#include "../res/app_res.h"
#include <pthread.h>

void *func(void *arg)
{
    fileset *set = (fileset *)arg;
    while (1)
    {
        _file_ *f = res_search(set, "K:\\CCXXProgram\\appon\\readme");
        if (f != NULL)
        {
            fprintf(stdout,"%s\n", f->data.data);
            fflush(stdout);
            return NULL;
        }
        fprintf(stdout,"NULL\n");
        fflush(stdout);
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    file_path files[1];

    files[0].active = 2;
    files[0].mode = "rb+";
    files[0].path = "K:\\CCXXProgram\\appon\\readme";

    //pthread_rwlock_t main_lock;
    fileset set;

    app_res(&set,files,1);
    func(&set);

    pthread_t  t;
    pthread_create(&t,NULL,func,&set);
    pthread_join(t,NULL);

    return 0;
}