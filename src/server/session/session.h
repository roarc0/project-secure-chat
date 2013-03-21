#ifndef _SESSION_H
#define _SESSION_H

#include "../../shared/session/sessionbase.h"
#include "packetfilter.h"

class Session : public SessionBase
{
    public:
        Session(SocketBase* pSock);
        ~Session();

        bool Update(uint32 diff, PacketFilter& updater);

        // THREADUNSAFE
        void KickSession();
        void SetId(uint32 id) { m_id = id; }

        // THREADSAFE 
        bool IsInChannel() { return channel_name == "" ? false : true; }        
        uint32 GetId() { return m_id; }
        void SendWaitQueue(int position);
        void SetInQueue(bool state) { m_inQueue = state; }

        // Handle
        void Handle_ServerSide(Packet& packet);
        void HandleMessage(Packet& packet); 
        void HandleJoinChannel(Packet& packet);       
  
    private:
        uint32 m_id;

        bool m_inQueue;
        // Channel
        std::string channel_name;
};

typedef counted_ptr<Session> Session_smart;

#endif
