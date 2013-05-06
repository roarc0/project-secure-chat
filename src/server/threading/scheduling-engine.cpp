#include "scheduling-engine.h"

void MethodThread::Execute(void* /*arg*/)
{
    while(1)
    {        
        MethodRequest* meth = sched_engine->GetNextMethod();
        if (!meth)
        {   
            INFO ("debug", "METHODTHREAD: method NULL\n");
            continue;
        }

        meth->Call();

        delete meth;
    }
}

SchedulingEngine::SchedulingEngine(): sem(m_mutex), b_active(false)
{

}

SchedulingEngine::~SchedulingEngine()
{
    while (!m_threads.empty())
    {
        delete *(m_threads.begin());
        m_threads.erase(m_threads.begin());
    }   
}

int SchedulingEngine::Initialize(uint32 n_thread)
{
    for (uint32 i = 0; i < n_thread; i++)
    {
        MethodThread* m_thread = new MethodThread(this); 
        if (!m_thread)
            return -1;

        if (m_thread->Start(NULL) != 0)
        {
            INFO ("debug", "SCHED_ENGINE: can't start scheduling engine!\n");
            continue;   
        }   
        m_threads.push_front(m_thread);
    }
    b_active = true;

    return 0;
}

int SchedulingEngine::Deactivate()
{
    // TODO
    b_active = false;
    return 0;
}

bool SchedulingEngine::IsActive()
{
    return b_active;
}

int SchedulingEngine::Execute(MethodRequest* m_req)
{   
    if (!m_req)
        return -1;

    q_method.add(m_req);    
    sem.Signal();  
    return 0;
}

MethodRequest* SchedulingEngine::GetNextMethod()
{
    sem.Wait();
    MethodRequest* ret = NULL;
    q_method.next(ret);
    return ret;
}
