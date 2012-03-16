#ifndef _SESSION_BASE_H
#define _SESSION_BASE_H

#include "../networking/packetfilter.h"
#include "../networking/packet.h"
#include "../queues/lock_queue.h"
#include "../threading/lock.h"

class Socket;

class SessionBase
{
    public:
        SessionBase(/*Socket* pSock*/);
        virtual ~SessionBase();

        // THREADSAFE
        void QueuePacket(Packet* new_packet); 
        void SendPacket(Packet* new_packet);

        void Handle_NULL(Packet& packet) {};

        virtual bool IsInChannel() { return false; }

    private:
        Mutex m_mutex;        

        LockedQueue<Packet*> _recvQueue;
        // Socket
        //Socket* m_Socket;
};

#endif 
