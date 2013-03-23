#ifndef _CHANNEL_MNG_H
#define _CHANNEL_MNG_H

#include "common.h"
#include "threading/lock.h"
#include "session-manager.h"

#include <list>
#include <map>
#include <utility>

#include "channel.h"
#include "channel-updater.h"

#define MIN_CHANNEL_UPDATE_DELAY 100  // TODO config

NEWEXCEPTION(ChannelManagerException);

typedef UNORDERED_MAP<std::string, Channel*> mapChannel;

class ChannelManager
{
	public:
		ChannelManager();
		~ChannelManager();

        void Update (uint32 diff);
     
        ChannelUpdater* GetChannelUpdater() { return &m_updater; }

        // THREADSAFE
        Channel* FindChannel(const std::string& c_name);
        int CreateChannel(std::string& c_name);

        // THREADUNSAFE
        // E' thread unsafe se un altro thread sta lavorando sul puntatore del canale
        int RemoveChannel(std::string& c_name);

        void SetChannelUpdateInterval(uint32 t)
        {
            if (t > MIN_CHANNEL_UPDATE_DELAY)
                t = MIN_CHANNEL_UPDATE_DELAY;

            i_timer.SetInterval(t);
            i_timer.Reset();
        };

    private:
        Mutex m_mutex;
        mapChannel m_channels;
        IntervalTimer i_timer;        
        ChannelUpdater m_updater;
};

#endif
