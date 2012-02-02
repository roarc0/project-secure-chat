#ifndef _USERSESSION_H
#define _USERSESSION_H

#include "../common.h"
#include "../networking/socket.h"
//#include "../../shared/networking/packet/packet.h"

class Packet // Per farlo compilare
{

};

#include <exception>         // For exception class
#include <semaphore.h>       // For semaphore

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
        UserSession(TCPSocket* Socket);
        ~UserSession()
        {
            delete m_Socket;
            //while (Packet* packet = _recvQueue.pop_front())
            //    delete packet;
            //while (Packet* packet = _sendQueue.pop_front())
            //    delete packet;
        }

        void UpdatePacket();
        
        void QueuePacketToRecv(Packet* new_packet);
        int RecvSize();
        Packet* GetPacketFromRecv();
        void QueuePacketToSend(Packet* new_packet);
        int SendSize();
        Packet* GetPacketFromSend();
        
        void SetSecurity(uint8 security) { m_security = security; };
        uint8 GetSecurity() const { return m_security; };

        /*

        Funzioni elaborazione pacchetto specifiche sull'utente, tipo cambio livello moderazione ecc..

        */

        uint32 getId() const { return m_id; };
        void setId(unit32 id) { m_id = id; };

    private:
        uint32 m_id;
        uint8 m_security;
        TCPSocket* m_Socket;
        std::string m_Address;       

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
        
        /// Class used for managing encryption
        //Crypt m_Crypt;
};

#endif  /* _USERSESSION_H */
