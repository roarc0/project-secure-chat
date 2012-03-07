#if !defined _THREAD_BASE_H_
#define _SINGLETON_H_

#include <pthread>

class Thread
{
   public:
      Thread();
      int Start(void * arg);
   protected:
      int Run(void * arg);
      static void * EntryPoint(void*);
      virtual void Setup();
      virtual void Execute(void*);
      void * Arg() const {return Arg_;}
      void Arg(void* a){Arg_ = a;}
   private:
      THREADID ThreadId_;
      void * Arg_;

};

Thread::Thread() {}

int Thread::Start(void * arg)
{
   Arg(arg); // store user data
   int code = thread_create(Thread::EntryPoint, this, & ThreadId_);
   return code;
}

int Thread::Run(void * arg)
{
   Setup();
   Execute( arg );
}

/*static */
void * Thread::EntryPoint(void * pthis)
{
   Thread * pt = (Thread*)pthis;
   pthis->Run( Arg() );
}

virtual void Thread::Setup()
{
        // Do any setup here
}

virtual void Thread::Execute(void* arg)
{
        // Your code goes here
}

#endif 
