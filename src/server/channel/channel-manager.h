#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "../../shared/common.h"
#include "../session/session-manager.h"

#include <list>
#include <map>
#include <tr1/unordered_map>
#include <exception>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "channel-updater.h"

class ChannelException : public exception 
{
	public:
		ChannelException(const std::string &message, bool inclSysMsg = false) throw();
		~ChannelException() throw();

		const char *what() const throw();

	private:
		std::string userMessage;  // Exception message
};

class ChannelManagerException : public exception 
{
	public:
		ChannelManagerException(const std::string &message, bool inclSysMsg = false) throw();
		~ChannelManagerException() throw();

		const char *what() const throw();

	private:
		std::string userMessage;  // Exception message
};

class Channel
{
    public:
        Channel();
        ~Channel();
    private:
};

#define UNORDERED_MAP std::tr1::unordered_map
typedef UNORDERED_MAP<std::string, Channel*> mapChannel;


class ChannelManager
{
	public:
		ChannelManager();
		~ChannelManager();

        Channel* FindChannel(std::string c_name) const;
        
        ChannelUpdater* GetChannelUpdater() { return &m_updater; }
    private:
        IntervalTimer i_timer;
        mapChannel m_channels;
        ChannelUpdater m_updater;
};

#endif
