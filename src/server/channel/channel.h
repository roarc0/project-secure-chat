#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "../../shared/common.h"
#include "../../shared/threading/lock.h"
#include "../session/session.h"

typedef UNORDERED_MAP<uint32, Session*> mapSession;
typedef std::pair<uint32, Session*> mapSession_pair;

NEWEXCEPTION(ChannelException);

class Channel
{
    public:
        Channel(std::string& c_name);
        ~Channel();

        void Update(uint32 t_diff);
        bool DelayedUpdate(uint32 t_diff);
        
        // THREADSAFE
        int SetName(std::string& c_name);
        std::string GetName() const { return name; }
        bool CanSessionEnter(Session* ses) const { return true; }

        void SendToAll(Packet* packet); 
        void SendToAllButOne(Packet* packet, uint32 id);
        void SendToOne(Packet* packet, uint32 id);

        void MakeChannelChangeName(Packet* packet);

        // THREADSAFE
        Session* FindSession(uint32 id);
        int  AddSession(Session* ses);

        //THREADUNSAFE 
        int  RemoveSession(uint32 id);    
    private:

        Mutex m_mutex;
        mapSession m_sessions;
        std::string name;
        bool b_todelete;
};

#endif
