#include "channel-updater.h"

class ChannelUpdateRequest: class MethodRequest
{
    private:

        Channel& m_channel;
        ChannelUpdater& m_updater;
        uint32 m_diff;

    public:

        ChannelUpdateRequest(Channel& m, ChannelUpdater& u, uint32 d)
            : m_channel(m), m_updater(u), m_diff(d)
        {
            
        }

        ~ChannelUpdateRequest()
        {

        }

        virtual int Call()
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
    Deactivate();
}

int ChannelUpdater::ScheduleUpdate(Channel& channel, uint32 diff)
{
    Lock guard(m_mutex);
    ++pending_requests;

    if (s_sched_engine->Execute(new ChannelUpdateRequest(channel, *this, diff)) == -1)
    {
        // TODO Log Errore

        --pending_requests;
        return -1;
    }

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

bool ChannelUpdater::IsActive()
{
    return s_sched_engine->IsActive();
}

// Chiamato dal Thread avviato dal s_sched_engine quando ha completato l'operazione
void ChannelUpdater::UpdateFinished()
{
    Lock guard(m_mutex);

    --pending_requests;
}
