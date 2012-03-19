#ifndef _SESSION_BASE_H
#define _SESSION_BASE_H

#include "../networking/packet.h"
#include "../common.h"

// DA RIMUOVERE QUANDO COMPILA SOCKET
class Socket
{
    public:
        Socket() {}
        void CloseSocket() {}
        int SendPacket(Packet& new_packet) { return 0; }
        bool IsClosed() { return false; }
};

class SessionBase
{
    public:
        SessionBase(Socket* pSock);
        virtual ~SessionBase();

        // THREADSAFE
        void QueuePacket(Packet* new_packet); 
        void SendPacket(Packet* new_packet);

        void Handle_NULL(Packet& packet) {};

        virtual bool IsInChannel() { return false; }

    protected:
        Mutex m_mutex;        

        LockedQueue<Packet*> _recvQueue;
        // Socket
        Socket* m_Socket;
};

#endif 
