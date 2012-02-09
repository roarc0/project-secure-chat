#include "thread.h"

pthread_t start_thread(void* (*thread_function)(void*) , void* t_params)
{
    pthread_t      tid;
    pthread_attr_t tattr;
    int ret;

    ret = pthread_attr_init(&tattr);
    ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

    if (ret = pthread_create(&tid, &tattr, thread_function, (void*)t_params))
    {
        perror("pthread_create");
        //delete t_params;  // FIXME se fallisce pthread create t_params resta in memoria...
        return (pthread_t) 0;
    }

    pthread_attr_destroy(&tattr);
    return tid;
}
