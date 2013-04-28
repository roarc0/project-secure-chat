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
        
        std::string GetNickname();
        void SetNickname(const std::string&);
        
        bool IsEncrypted() const
        {
            return s_enc != enc_NONE;
        }
        
        void SetEncryption(const ByteBuffer& key, SessionEncryption type=enc_AES128)
        {
            s_key = key;
            s_enc = type;
        }

        virtual bool IsInChannel() { return false; } // serve anche al client?

        // Handle
        void Handle_NULL(Packet& /*packet*/);

        // Socket
        SocketBase* m_Socket;
    protected:

        virtual int _SendPacket(Packet& new_packet);
        int _SendPacketToSocket(Packet& pct);
        Packet* _RecvPacketFromSocket();
        
        std::string nickname;

        Mutex m_mutex;

        SessionEncryption s_enc;
        ByteBuffer s_key;

        LockQueue<Packet*> _recvQueue;
        LockQueue<Packet*> _sendQueue;

        Packet* pct;
        char* buffer;
};

#endif
