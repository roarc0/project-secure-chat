#include "thread.h"

Thread::Thread() {}

virtual Thread::~Thread() {}

int Thread::Start(void* arg)
{
   Arg(arg); // store user data
   int code = pthread_create(Thread::EntryPoint, this, &tid);
   return code;
}

int Thread::Run(void* arg)
{
   Setup();
   Execute(arg);
}

void * Thread::EntryPoint(void * pthis)
{
   Thread * pt = (Thread*)pthis;
   pt->Run( pt->Arg() );
}

virtual void Thread::Setup()
{
    int ret;
    pthread_attr_t tattr;

    ret = pthread_attr_init(&tattr);
    ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    pthread_attr_destroy(&tattr);
}

virtual void Thread::Execute(void* arg)
{

}

#endif 
