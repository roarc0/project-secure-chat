#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "../../shared/common.h"

#include <list>
#include <map>
#include <tr1/unordered_map>
#include <exception>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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
        Channel(uint32 owner, std::string name, std::string password = "", uint8 secure = 0, bool persistent = false);
        ~Channel();
        std::list<uint32> user_list;        
        std::string m_name;    

        void ResetTime() { gettimeofday(&m_createTime, NULL); }        
        uint32 GetTime(); // In millisecondi
    
        bool IsSecure() { return m_secure != 0; }
        uint32 GetOwner() { return m_owner; }
        void SetOwner(uint32 owner)  { m_owner = owner; }

        bool Access(uint32 id, std::string password, uint8 secure) throw(ChannelException);
        bool Exit(uint32 id, uint32& new_owner);
        bool SendPacketToAll(uint32 exclude_id = 0);
    private:
        timeval m_createTime;
        std::string m_password;
        uint8 m_secure;
        uint32 m_owner;
        bool m_persistent;

        pthread_mutex_t    mutex_Channel;

        inline void  getlock_Channel() { pthread_mutex_lock(&mutex_Channel); }
        inline void  releaselock_Channel() { pthread_mutex_unlock(&mutex_Channel); }
};

#define UNORDERED_MAP std::tr1::unordered_map
typedef UNORDERED_MAP<std::string, Channel*> mapChannel;


class ChannelManager
{
	public:
		ChannelManager();
		~ChannelManager();

        bool CreateChannel(uint32 id, std::string name, std::string password = "", 
                           uint8 secure = 0, bool persistent = false);
        bool AccessChannel(uint32 id, std::string name, std::string password = "", uint8 secure = 0);
        uint32 ExitChannel(uint32 id, std::string name);

        void SendPacketToChannel(std::string name, uint32 exclude_id = 0);

    private:
        mapChannel m_mapChannel;

        pthread_mutex_t    mutex_mapChannel;

        inline void  getlock_mapChannel() { pthread_mutex_lock(&mutex_mapChannel); }
        inline void  releaselock_mapChannel() { pthread_mutex_unlock(&mutex_mapChannel); }
};

#endif
