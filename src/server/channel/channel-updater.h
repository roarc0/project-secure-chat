#ifndef CHANNEL_UPDATER_H
#define CHANNEL_UPDATER_H

#include "common.h"
#include "channel.h"
#include "method-request.h"
#include "scheduling-engine.h"
#include "threading/lock.h"
#include "utility/counted_ptr.h"

NEWEXCEPTION(ChannelUpdaterException);

class ChannelUpdater
{
    //friend class ChannnelUpdateRequest;
    public:
        ChannelUpdater();
        ~ChannelUpdater();

        int ScheduleUpdate(Channel& channel, uint32 diff);

        int Wait();

        // int Activate(size_t num_threads);

        // int Deactivate();

        bool IsActivate();

        void UpdateFinished();

    private:        
        Mutex m_mutex;
        size_t pending_requests;        
};
#endif
