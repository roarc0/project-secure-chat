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
    enc_NONE = 0,
    enc_AES128,
    enc_AES256
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
            return s_enc != enc_NONE;
        }
        
        void SetEncryption(const ByteBuffer& key, SessionEncryption type=enc_AES128)
        {
            s_key = key;
            s_enc = type;
        }
        
        bool IsLogged() const
        {
            return logged;
        }

        virtual bool IsInChannel() { return false; } // serve anche al client?

        void Handle_NULL(Packet& /*packet*/);
        SocketBase* m_Socket;
    protected:

        bool SetLogged(bool l)
        {
            logged = l;
        }

        virtual int _SendPacket(Packet& new_packet);
        int _SendPacketToSocket(Packet& pct);
        Packet* _RecvPacketFromSocket();
        
        std::string username;
        bool logged;

        Mutex m_mutex;

        SessionEncryption s_enc;
        ByteBuffer s_key;

        LockQueue<Packet*> _recvQueue;
        LockQueue<Packet*> _sendQueue;

        Packet* pct;
        char* buffer;
};

#endif
