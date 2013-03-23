#ifndef _SESSION_BASE_H
#define _SESSION_BASE_H

#include "networking/packet.h"
#include "networking/socket-base.h"
#include "utility/queues/lock_queue.h"
#include "threading/lock.h"
#include "threading/semaphore.h"
#include "common.h"

class SessionBase
{
    public:
        SessionBase();
        SessionBase(int pSock);
        virtual ~SessionBase();

        // THREADSAFE
        void QueuePacket(Packet* new_packet); 
        void SendPacket(Packet* new_packet);
        Packet* GetPacketToSend();     

        virtual bool IsInChannel() { return false; } // serve anche al client?

        // Handle
        void Handle_NULL(Packet& /*packet*/);
        
        // Socket
        SocketBase* m_Socket;
    protected:

        int _SendPacket(const Packet& new_packet);

        Mutex m_mutex;        

        LockedQueue<Packet*> _recvQueue;
        LockedQueue<Packet*> _sendQueue;
    
};

#endif 
