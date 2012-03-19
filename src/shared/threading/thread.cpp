#include "thread.h"

Thread::Thread() : tid_(0)
{

}

Thread::~Thread() 
{
    if (tid_)
        pthread_kill(tid_, SIGKILL);
}

int Thread::Start(void* arg)
{
    Arg(arg); // store user data
    int ret;
    pthread_attr_t tattr;

    ret = pthread_attr_init(&tattr);
    if (ret != 0)
        return ret;
    ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    if (ret != 0)
        return ret;
    ret = pthread_create(&tid_, &tattr, Thread::EntryPoint, this);
    if (ret != 0)
        return ret;

    ret = pthread_attr_destroy(&tattr);
    return ret;
}

void Thread::Run(void* arg)
{
    Setup();
    Execute(arg);
}

void* Thread::EntryPoint(void* pthis)
{
   Thread* pt = (Thread*)pthis;
   pt->Run(pt->Arg());
   return NULL;  
}

void Thread::Setup()
{

}

void Thread::Execute(void* /*arg*/)
{

}

