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
        void SendPacketToSocket(Packet* new_packet);
        Packet* RecvPacketFromSocket();
        Packet* GetPacketToSend();     

        virtual bool IsInChannel() { return false; } // serve anche al client?

        // Handle
        void Handle_NULL(Packet& /*packet*/);
        
        // Socket
        SocketBase* m_Socket;
    protected:

        virtual int _SendPacket(const Packet& new_packet);
        int _SendPacketToSocket(const Packet& pct);
        Packet* _RecvPacketFromSocket();

        Mutex m_mutex;        

        LockedQueue<Packet*> _recvQueue;
        LockedQueue<Packet*> _sendQueue;

        Packet* recVpkt;
        char* buffer;    
};

#endif 
