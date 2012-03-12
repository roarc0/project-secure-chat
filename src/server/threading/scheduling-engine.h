#ifndef _SCHED_MANAGER_H
#define _SCHED_MANAGER_H

#include "../../shared/typedefs.h"
#include "../../shared/queues/lock_queue.h"
#include "../../shared/threading/lock.h"
#include "../../shared/threading/semaphore.h"
#include "method-request.h"

#define s_sched_engine      SchedulingEngine::GetInstance()

class SchedulingEngine : public Singleton
{
    public:
        void Initialize(uint32 n_thread);
        int Deactivate();
        bool IsActive();
        int Execute(MethodRequest* m_req);
    
        MethodRequest* GetNextMethod();

    private:
        SchedulingEngine();
        ~SchedulingEngine();

        LockedQueue<MethodRequest*> q_method;

        bool b_active;

        Mutex m_mutex;
        Semaphore sem;
};


#endif
