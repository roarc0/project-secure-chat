#ifndef _SESSION_BASE_H
#define _SESSION_BASE_H

#include "networking/packet.h"
#include "networking/socket-base.h"
#include "utility/queues/lock_queue.h"
#include "threading/lock.h"
#include "threading/semaphore.h"
#include "common.h"

#define MAX_USER_LEN    16
#define EXCLUDED_CHARS  " \t\f\v\n\r"
#define NONCE_SIZE      32

enum SessionEncryption
{
    ENC_UNSPEC = 0,
    ENC_NONE,
    ENC_AES128,
    ENC_AES256,
    ENC_RSA,
    ENC_HYB
};

enum SessionStatus
{
    STATUS_NONE = 0,    
    STATUS_REJECTED,
    STATUS_DISCONNECTED,
    STATUS_CONNECTED,
    STATUS_LOGIN_STEP_1,
    STATUS_LOGIN_STEP_2,
    STATUS_LOGIN_STEP_3,
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
        
        void SetEncryption(const ByteBuffer&, SessionEncryption);
        void SetEncryption(SessionEncryption type);
        void SetNextEncryption(SessionEncryption type);
        bool IsEncrypted() const;
        bool IsSymmetric() const;
        
        bool IsConnected() const;
        bool IsRejected() const;
        bool IsAuthenticated() const;
        void SetConnected(bool); // will be deprecated
        SessionStatus GetSessionStatus() const;
        void SetSessionStatus(SessionStatus);
        
        virtual bool IsInChannel() { return false; }

        const char* GetPassword();
        void SetPassword(const char *);
        bool HavePassword();
        void ClearPassword();
        
        void GenerateNonce();
        bool CheckNonce(const uint8*);
        bool CheckNonce(const ByteBuffer&);
        void SetOtherNonce(const uint8*);

        // Resetta la numerazione dei pacchetti
        void ResetPacketNum();
        
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
        SessionEncryption s_enc, s_next_enc;
        
        /* asymmethric encryption */
        ByteBuffer s_my_nonce, s_other_nonce;
        string s_pwd;
        string f_my_pub_key, f_my_priv_key,
               f_other_pub_key;

        uint32 test_data;
        
        /* symmethric encryption */
        ByteBuffer s_key, s_key_tmp;
        uint8 u_changekeys;

        uint32 u_id_send;
        uint32 u_id_receive;

        Mutex m_mutex;

        LockQueue<Packet*> _recvQueue;
        LockQueue<Packet*> _sendQueue;
};

#endif
