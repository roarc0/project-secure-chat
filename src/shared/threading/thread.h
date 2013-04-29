#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>
#include <signal.h>

class Thread
{
   public:
      Thread();
      virtual ~Thread();
      int Start(void * arg);
   protected:
      void Run(void * arg);
      static void* EntryPoint(void*);
      virtual void Setup();
      virtual void Execute(void*);
      virtual void Exit();
      void * Arg() const {return arg_;}
      void Arg(void* a){arg_ = a;}
      
      void * arg_;

    private:
      pthread_t tid_;
};

int StartThread(void* (*fptr)(void*), void *data, pthread_t& tid);

#endif
