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

enum SessionLogin
{
    LOGIN_INIT = 0,
    LOGIN_STEP_1,
    LOGIN_STEP_2,
    LOGIN_AUTHENTICATED
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
        
        bool IsEncrypted() const
        {
            return s_enc != ENC_NONE;
        }
        
        void SetEncryption(const ByteBuffer& key, SessionEncryption type=ENC_AES128)
        {
            s_key = key;
            s_enc = type;
        }
        
        bool IsAuthenticated() const
        {
            return s_login == LOGIN_AUTHENTICATED;
        }
        
        SessionLogin GetLoginStatus() const
        {
            return s_login;
        }

        virtual bool IsInChannel() { return false; } // serve anche al client?

        void Handle_NULL(Packet& /*packet*/);
        
        SocketBase* m_Socket; // TODO protected
    protected:

        void SetLoginStatus(SessionLogin l)
        {
            s_login = l;
        }

        virtual int _SendPacket(Packet& new_packet);
        int _SendPacketToSocket(Packet& pct);
        Packet* _RecvPacketFromSocket();
        
        std::string username;
        SessionLogin s_login;
        SessionEncryption s_enc;
        ByteBuffer s_key;
        
        Mutex m_mutex;

        LockQueue<Packet*> _recvQueue;
        LockQueue<Packet*> _sendQueue;

        Packet* pct;
        char* buffer;
};

#endif
