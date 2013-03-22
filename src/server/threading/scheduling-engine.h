#ifndef _SCHEDULING_ENGINE_H
#define _SCHEDULING_ENGINE_H

#include "../../shared/common.h"
#include "../../shared/threading/thread.h"
#include "../../shared/utility/singleton.h"
#include "method-request.h"
#include <list>

class SchedulingEngine;

NEWEXCEPTION(SchedulingException);

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
        SchedulingEngine();
        ~SchedulingEngine();

        int Initialize(uint32 n_thread);
        int Deactivate();
        bool IsActive();
        int Execute(MethodRequest* m_req);

        uint32 Count() const
        {
            return m_threads.size();
        }
    
        MethodRequest* GetNextMethod();        

    private:
        std::list<MethodThread*> m_threads;

        LockedQueue<MethodRequest*> q_method;

        bool b_active;

        Mutex m_mutex;
        Semaphore sem;
};

#define s_sched_engine Singleton<SchedulingEngine>::GetInstance()

#endif
