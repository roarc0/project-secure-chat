#ifndef _SESSION_BASE_H
#define _SESSION_BASE_H

#include "networking/packet.h"
#include "networking/socket-base.h"
#include "utility/queues/lock_queue.h"
#include "threading/lock.h"
#include "threading/semaphore.h"
#include "common.h"

enum SessionEncryption
{
    ENC_NONE = 0,
    ENC_AES128,
    ENC_AES256
};

enum SessionStatus
{
    STATUS_DISCONNECTED = 0,
    STATUS_CONNECTED,
    STATUS_LOGIN_INIT,
    STATUS_LOGIN_STEP_1,
    STATUS_LOGIN_STEP_2,
    STATUS_AUTHENTICATED,
    STATUS_REJECTED,
    STATUS_NONE
};

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
        
        const std::string* GetUsername();
        void SetUsername(const std::string&);
        bool IsEncrypted() const;
        void SetEncryption(const ByteBuffer&, SessionEncryption);
        bool IsAuthenticated() const;
        bool IsConnected() const;
        void SetConnected(bool);
        SessionStatus GetSessionStatus() const;
        
        virtual bool IsInChannel() { return false; } // serve anche al client?

        void Handle_NULL(Packet& /*packet*/);
        
        SocketBase* m_Socket; // TODO protected
    protected:

        void SetLoginStatus(SessionStatus l)
        {
            s_status = l;
        }

        virtual int _SendPacket(Packet& new_packet);
        int _SendPacketToSocket(Packet& pct);
        Packet* _RecvPacketFromSocket();
        
        std::string username;
        SessionStatus s_status;
        SessionEncryption s_enc;
        ByteBuffer s_key;
        
        Mutex m_mutex;

        LockQueue<Packet*> _recvQueue;
        LockQueue<Packet*> _sendQueue;

        Packet* pct;
        char* buffer;
};

#endif
