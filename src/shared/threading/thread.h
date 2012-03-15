#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>

class Thread
{
   public:
      Thread();
      virtual ~Thread();
      int Start(void * arg);
   protected:
      int Run(void * arg);
      static void * EntryPoint(void*);
      virtual void Setup();
      virtual void Execute(void*);
      void * Arg() const {return arg_;}
      void Arg(void* a){arg_ = a;}
      
      void * arg_;

    private:
      pthread_t tid_;
};

#endif
