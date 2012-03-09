#include "thread.h"

Thread::Thread() {}

int Thread::Start(void * arg) throw(ThreadException)
{
   int ret;

   SetArg(arg);
   if(ret = pthread_create(Thread::EntryPoint, this, & ThreadId_) < 0)
       throw(ThreadException);
   return ret;
}

int Thread::Run(void * arg) throw(ThreadException)
{
   Setup();
   Execute(arg);
}

void * Thread::EntryPoint(void * pthis) throw(ThreadException)
{
   Thread * pt = (Thread*)pthis;
   pthis->Run(Arg());
}

virtual void Thread::Setup() throw(ThreadException)
{
    int ret;
    pthread_attr_t tattr;

    ret = pthread_attr_init(&tattr);
    ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    pthread_attr_destroy(&tattr);
}

virtual void Thread::Execute(void* arg) throw(ThreadException)
{
}

#endif 
