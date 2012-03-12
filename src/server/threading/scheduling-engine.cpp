SchedulingEngine* SchedulingEngine::_instance = NULL;

SchedulingEngine::SchedulingEngine() : b_active(false)
{

}

SchedulingEngine::~SchedulingEngine()
{

}

void SchedulingEngine::Initialize(uint32 n_thread)
{

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
    return 0;
}
