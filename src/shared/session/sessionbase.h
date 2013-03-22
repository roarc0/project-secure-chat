#ifndef _SESSION_BASE_H
#define _SESSION_BASE_H

#include "../networking/packet.h"
#include "../networking/socket-base.h"
#include "../common.h"

class SessionBase
{
    public:
        SessionBase(SocketBase* pSock);
        virtual ~SessionBase();

        // THREADSAFE
        void QueuePacket(Packet* new_packet); 
        void SendPacket(Packet* new_packet);
        Packet* GetPacketToSend();     

        virtual bool IsInChannel() { return false; }

        // Handle
        void Handle_NULL(Packet& /*packet*/);

    protected:

        int _SendPacket(const Packet& new_packet);

        Mutex m_mutex;        

        LockedQueue<Packet*> _recvQueue;
        LockedQueue<Packet*> _sendQueue;
        // Socket
        SocketBase* m_Socket;    
};

#endif 
