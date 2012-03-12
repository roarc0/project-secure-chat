#ifndef _SCHED_MANAGER_H
#define _SCHED_MANAGER_H

#include "../../shared/typedefs.h"
#include "method-request.h"

#define s_sched_engine      SchedulingEngine::GetInstance()

class SchedulingEngine : public Singleton
{
    public:
        void Initialize(uint32 n_thread);
        int Deactivate();
        bool IsActive();
        int Execute(MethodRequest* m_req);

    private:
        SchedulingEngine();
        ~SchedulingEngine();

        bool b_active;

        Mutex lock;

};


#endif
