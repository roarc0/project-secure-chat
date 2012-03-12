#ifndef CHANNEL_UPDATER_H
#define CHANNEL_UPDATER_H

#include "../threading/method-request.h"
#include "../threading/scheduling-engine.h"
#include "../../shared/threading/lock.h"
#include "../../shared/common.h"

class Channel;

class ChannelUpdater
{
    friend class ChannnelUpdateRequest;
    public:
        ChannelUpdater();
        ~ChannelUpdater();

        int ScheduleUpdate(Channel& channel, uint32 diff);

        int Wait();

        // int Activate(size_t num_threads);

        // int Deactivate();

        bool IsActive();

    private:        
        Mutex m_mutex;
        size_t pending_requests;

        void UpdateFinished();
};
#endif
