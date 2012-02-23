#include "../../server/session/session-manager.h"

#ifndef _USERSESSION_H
#define _USERSESSION_H

#include "../common.h"
#include "../networking/socket.h"
#include "../networking/packet.h"

#include <exception>         // For exception class
#include <semaphore.h>       // For semaphore

class Session;

class UserSessionException : public exception 
{
    public:
        UserSessionException(const std::string &message, bool inclSysMsg = false) throw();
        ~UserSessionException() throw();

        const char *what() const throw();

    private:
        std::string userMessage;  // Exception message
};

// Classe di sessione dell'utente con i parametri di esso
class UserSession
{
    public:
        UserSession(TCPSocket* Socket, Session* pSes = NULL);
        ~UserSession()
        {
            delete m_Socket;
            pthread_mutex_destroy(&mutex_recv);
            pthread_mutex_destroy(&mutex_send); 
            while (!_recvQueue.empty())
            {
                delete _recvQueue.front();
                _recvQueue.pop_front();
            }
            while (!_sendQueue.empty())
            {
                delete _sendQueue.front();
                _sendQueue.pop_front();
            }
        }

        void UpdatePacket();
        
        void QueuePacketToRecv(Packet* new_packet);
        int RecvSize();
        Packet* GetPacketFromRecv();
        void QueuePacketToSend(Packet* new_packet);
        int SendSize();
        Packet* GetPacketFromSend();
        
        void SetSecurity(uint8 security) { m_security = security; }
        uint8 GetSecurity() const { return m_security; }

        void ResetTime() { gettimeofday(&m_createTime, NULL); } 
        uint32 GetTime() // In millisecondi
        {
            uint32 seconds  = m_createTime.tv_sec;  // - start.tv_sec;  TODO Quando è stato avviato il programma
            uint32 useconds = m_createTime.tv_usec; // - start.tv_usec; TODO Quando è stato avviato il programma
            return ((seconds) * 1000 + useconds/1000.0) + 0.5;
        }

        void releaselock_net();
        void releaselock_exec();

        /*

        Funzioni elaborazione pacchetto specifiche sull'utente, tipo cambio livello moderazione ecc..

        */
        std::string GetName() const { return m_Name; }
        void SetName(std::string name) { name = m_Name; }

        std::string GetChannel() const { return m_Channel; }
        void SetChannel(std::string channel) { channel = m_Channel; }

        uint32 GetId() const { return m_id; }
        void SetId(uint32 id) { m_id = id; }

        void SetSession(Session* pSes) { m_pSes = pSes; }

        TCPSocket* GetSocket() { return m_Socket; }

    private:
        Session* m_pSes;

        uint32 m_id;
        uint8 m_security;
        TCPSocket* m_Socket;
        std::string m_Name;
        std::string m_Address;
        std::string m_Channel;

        list<Packet*> _recvQueue;
        list<Packet*> _sendQueue;

        // Mutex
        pthread_mutex_t    mutex_recv;
        pthread_mutex_t    mutex_send;

        inline void MutexInit()
        {
            pthread_mutex_init(&mutex_recv, NULL);
            pthread_mutex_init(&mutex_send, NULL);
        }

        inline void  getlock_recv() { pthread_mutex_lock(&mutex_recv); }
        inline void  releaselock_recv() { pthread_mutex_unlock(&mutex_recv); }

        inline void  getlock_send() { pthread_mutex_lock(&mutex_send); }
        inline void  releaselock_send() { pthread_mutex_unlock(&mutex_send); }  


    private:
        timeval m_createTime;
        
        /// Class used for managing encryption
        //Crypt m_Crypt;
};

#endif  /* _USERSESSION_H */
