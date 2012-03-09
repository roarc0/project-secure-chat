#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "../../shared/common.h"
#include "../session/session-manager.h"

#include <list>
#include <map>
#include <utility>

#include "channel.h"
#include "channel-updater.h"

class ChannelManagerException : public exception 
{
	public:
		ChannelManagerException(const std::string &message, bool inclSysMsg = false) throw();
		~ChannelManagerException() throw();

		const char *what() const throw();

	private:
		std::string userMessage;  // Exception message
};

typedef UNORDERED_MAP<std::string, Channel*> mapChannel;

class ChannelManager
{
	public:
		ChannelManager();
		~ChannelManager();

        void Update (uint32 diff);   
     
        ChannelUpdater* GetChannelUpdater() { return &m_updater; }

        // THREADSAFE
        Channel* FindChannel(std::string& c_name) const;
        Channel* CreateChannel(std::string& c_name);

        bool CanSessionEnter(Session* ses, std::string& c_name) const;

        // THREADUSAFE
        // E' thread unsafe se un altro thread sta lavorando sul puntatore del canale
        Channel* RemoveChannel(std::string& c_name); 

    private:
        Mutex m_mutex;
        mapChannel m_channels;
        IntervalTimer i_timer;        
        ChannelUpdater m_updater;
};

#endif
