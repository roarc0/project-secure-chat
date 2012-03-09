#ifndef CHANNEL_UPDATER_H
#define CHANNEL_UPDATER_H

#include "../threading/method-request.h"

//SchedulingEngine s_sched_engine;

class ChannelUpdater
{
    friend class ChannnelUpdateRequest;
    public:
        ChannelUpdater();
        ~ChannelUpdater();

        int ScheduleUpdate(Channel& channel, uint32 diff);

        int Wait();

        int Activate(size_t num_threads);

        int Deactivate();

        bool IsActive();

    private:        
        Mutex m_mutex;
        size_t pending_requests;

        void UpdateFinished();
};
#endif
