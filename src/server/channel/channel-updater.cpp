#include "channel-updater.h"

class ChannelUpdateRequest: public MethodRequest
{
    private:

        Channel& m_channel;
        ChannelUpdater& m_updater;
        uint32 m_diff;

    public:

        ChannelUpdateRequest(Channel& m, ChannelUpdater& u, uint32 d) : 
        MethodRequest(), m_channel(m), m_updater(u), m_diff(d)
        {
            
        }

        ~ChannelUpdateRequest()
        {

        }

        int Call()
        {
            m_channel.Update(m_diff);
            m_updater.UpdateFinished();
            return 0;
        }
};

ChannelUpdater::ChannelUpdater() :
pending_requests(0)
{

}

ChannelUpdater::~ChannelUpdater()
{
    // Deactivate();
}

int ChannelUpdater::ScheduleUpdate(Channel& channel, uint32 diff)
{
    Lock guard(m_mutex);    

    if (s_sched_engine->Execute(new ChannelUpdateRequest(channel, *this, diff)) != 0)
    {
        // TODO Log Errore
        return -1;
    }

    ++pending_requests;

    return 0;
}

int ChannelUpdater::Wait()
{
    while (pending_requests > 0)
        sleep(0);

    return 0;
}

/*int ChannelUpdater::Activate(size_t num_threads)
{
    return s_sched_engine->Activate((int)num_threads);
}*/

/*int ChannelUpdater::Deactivate()
{    
    Wait();

    return s_sched_engine->Deactivate();
}*/

bool ChannelUpdater::IsActivate()
{
    return s_sched_engine->IsActive();
}

// Chiamato dal Thread avviato dal s_sched_engine quando ha completato l'operazione
void ChannelUpdater::UpdateFinished()
{
    Lock guard(m_mutex);

    --pending_requests;
}
