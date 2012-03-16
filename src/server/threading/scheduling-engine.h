#ifndef _SCHED_MANAGER_H
#define _SCHED_MANAGER_H

#include "../../shared/common.h"
#include "../../shared/queues/lock_queue.h"
#include "../../shared/threading/lock.h"
#include "../../shared/threading/semaphore.h"
#include "../../shared/threading/thread.h"
#include "../../shared/singleton.h"
#include "method-request.h"
#include <list>

class SchedulingEngine;

class MethodThread : public Thread
{
    public:
        MethodThread(SchedulingEngine* sched): Thread() { sched_engine = sched; }
        ~MethodThread() {}

        void Execute(void* arg);
        void Setup() {}
    private:
        SchedulingEngine* sched_engine;
};

class SchedulingEngine
{
    friend class Singleton<SchedulingEngine>;
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

        std::list<MethodThread*> m_threads;

        LockedQueue<MethodRequest*> q_method;

        bool b_active;

        Mutex m_mutex;
        Semaphore sem;
};

#define s_sched_engine Singleton<SchedulingEngine>::GetInstance()

#endif
