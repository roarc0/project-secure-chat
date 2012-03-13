#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "../../shared/common.h"
#include "../../shared/threading/lock.h"
#include "../session/session.h"



class ChannelException : public exception 
{
	public:
		ChannelException(const std::string &message, bool inclSysMsg = false) throw();
		~ChannelException() throw();

		const char *what() const throw();

	private:
		std::string userMessage;  // Exception message
};

typedef UNORDERED_MAP<id, Session*> mapSession;

class Channel
{
    public:
        Channel(std::string& c_name);
        ~Channel();

        void Update(uint32 t_diff);
        bool DelayedUpdate(uint32 t_diff);
        
        // THREADSAFE
        int SetName(std::string& c_name);
        std::string GetName() { return name; }
        Session* FindSession(Session* ses);
        Session* FindSession(uint32 id);
        int  AddSession(Session* ses);        

        //THREADUNSAFE 
        int  RemoveSession(uint32 id);       
    private:

        Mutex m_mutex;
        mapSession m_sessions;
        std:string name;
        bool b_todelete;
};

#endif
