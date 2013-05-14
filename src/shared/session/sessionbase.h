#ifndef _SESSION_BASE_H
#define _SESSION_BASE_H

#include "networking/packet.h"
#include "networking/socket-base.h"
#include "utility/queues/lock_queue.h"
#include "threading/lock.h"
#include "threading/semaphore.h"
#include "common.h"

#define MAX_USER_LEN    32
#define EXCLUDED_CHARS  " \t\f\v\n\r"

enum SessionEncryption
{
    ENC_NONE = 0,
    ENC_AES128,
    ENC_AES256,
    ENC_RSA
};

enum SessionStatus
{
    STATUS_NONE = 0,    
    STATUS_REJECTED,
    STATUS_DISCONNECTED,
    STATUS_CONNECTED,
    STATUS_LOGIN_STEP_1,
    STATUS_LOGIN_STEP_2,
    STATUS_AUTHENTICATED
};

class SessionBase
{
    public:
        SessionBase();
        SessionBase(int pSock);
        virtual ~SessionBase();

        void Close();
        virtual bool IsServer() { return false; };

        // THREADSAFE
        void QueuePacket(Packet* new_packet);
        void SendPacket(Packet* new_packet);
        void SendPacketToSocket(Packet* new_packet, unsigned char* temp_buffer = NULL);
        Packet* RecvPacketFromSocket(unsigned char* temp_buffer = NULL);
        Packet* GetPacketToSend();
        
        const std::string* GetUsername();
        bool SetUsername(const std::string&);
        bool IsEncrypted() const;
        void SetEncryption(const ByteBuffer&, SessionEncryption);
        bool IsAuthenticated() const;
        bool IsConnected() const;
        bool IsRejected() const;
        void SetConnected(bool);
        SessionStatus GetSessionStatus() const;
        void SetSessionStatus(SessionStatus);
        
        virtual bool IsInChannel() { return false; }
        
        const char* GetPassword();
        void SetPassword(const char *);
        bool HavePassword();
        void ClearPassword();
        bool TestRsa();
        virtual void UpdateKeyFilenames(){ };
            
        void HandleNULL(Packet& /*packet*/);
        
        SocketBase* m_Socket;
        
    protected:
        virtual int _SendPacket(Packet& new_packet) = 0;
        int _SendPacketToSocket(Packet& pct, unsigned char* temp_buffer = NULL);
        Packet* _RecvPacketFromSocket(unsigned char* temp_buffer = NULL);

        std::string username;
        SessionStatus s_status;
        SessionEncryption s_enc;
        
        /* asymmethric encryption */
        string s_pwd;
        string f_my_pub_key, f_my_priv_key,
               f_other_pub_key;
        
        /* symmethric encryption */
        ByteBuffer s_key, s_key_tmp;
        uint8 u_changekeys;

        Mutex m_mutex;

        LockQueue<Packet*> _recvQueue;
        LockQueue<Packet*> _sendQueue;
};

#endif
