SchedulingEngine* SchedulingEngine::_instance = NULL;

SchedulingEngine::SchedulingEngine() : sem(m_mutex), b_active(false)
{

}

SchedulingEngine::~SchedulingEngine()
{

}

void SchedulingEngine::Initialize(uint32 n_thread)
{
    // Creazione N thread e passaggio di variabili
    b_active = true;
}

int SchedulingEngine::Deactivate()
{
    return 0;
}

bool SchedulingEngine::IsActive()
{
    return b_active;
}

int SchedulingEngine::Execute(MethodRequest* m_req)
{   
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
