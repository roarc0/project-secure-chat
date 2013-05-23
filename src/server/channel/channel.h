#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "common.h"
#include "threading/lock.h"
#include "utility/counted_ptr.h"
#include "xml-message.h"
#include "session.h"

class Session;
typedef counted_ptr<Session> Session_smart;
typedef UNORDERED_MAP<uint32, Session_smart> mapSession;
typedef std::pair<uint32, Session_smart> mapSession_pair;

class Channel;
typedef counted_ptr<Channel> SmartChannel;

NEWEXCEPTION(ChannelException);

class Channel
{
    public:
        Channel(std::string& c_name, std::string& c_pass);
        ~Channel();

        void Update(uint32 t_diff);
        bool DelayedUpdate(uint32 t_diff);
        
        // THREADSAFE
        int SetName(std::string& c_name);
        std::string GetName() const { return name; }
        bool CanEnterSession(Session_smart /*ses*/, std::string& /*pass*/) const;
        int getSessionNumer();

        void SendToAll(Packet* packet); 
        void SendToAllButOne(Packet* packet, uint32 id);
        void SendToOne(Packet* packet, uint32 id);

        void MakeChannelChangeName(Packet* packet);
        void SendUsersList(Session_smart ses);

        // THREADSAFE
        Session_smart FindSession(uint32 id);
        bool  AddSession(Session_smart ses);

        //THREADUNSAFE 
        bool  RemoveSession(uint32 id);    
    private:

        Mutex m_mutex;
        mapSession m_sessions;
        std::string name;
        std::string pass;
        bool b_todelete;
};

#endif
