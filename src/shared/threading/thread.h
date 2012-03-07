#ifndef _THREAD_H
#define _THREAD_H

#include <pthread>
#include "../utility/exception.h"

class ThreadException : public Exception;

class Thread
{
   public:
      Thread();
      ~Thread();
      int Start(void * arg) throw(ThreadException);
   protected:
      int Run(void * arg) throw(ThreadException);
      static void * EntryPoint(void*) throw(ThreadException);
      virtual void Setup() throw(ThreadException);
      virtual void Execute(void*) throw(ThreadException);;
      void* GetArg() const {return arg;}
      void SetArg(void* _arg){arg = _arg;}
   private:
      pthread_t tid;

      void      *arg;

};

#endif
