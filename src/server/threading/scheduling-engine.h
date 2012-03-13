#ifndef _SCHED_MANAGER_H
#define _SCHED_MANAGER_H

#include "../../shared/typedefs.h"
#include "../../shared/queues/lock_queue.h"
#include "../../shared/threading/lock.h"
#include "../../shared/threading/semaphore.h"
#include "method-request.h"
#include <list>

class SchedulingEngine;

struct thread_params
{
     SchedulingEngine *ptr;
};

class MethodThread : public Thread
{
    public:
    MethodThread() {};
    ~MethodThread() { delete arg_ };

    void Execute(void* arg);
    void Setup() {}
};

#define s_sched_engine      SchedulingEngine::GetInstance()

class SchedulingEngine : public Singleton
{
    public:
        void Initialize(uint32 n_thread);
        int Deactivate();
        bool IsActive();
        int Execute(MethodRequest* m_req);

        uint32 Count() const
        {
            return m_threads.size();
        }
    
        MethodRequest* GetNextMethod();

    private:
        SchedulingEngine();
        ~SchedulingEngine();

        list<MethodThreads*> m_threads;

        LockedQueue<MethodRequest*> q_method;

        bool b_active;

        Mutex m_mutex;
        Semaphore sem;
};


#endif
