#ifndef _USERSESSION_H
#define _USERSESSION_H

#include "../../shared/common.h"
#include "../../shared/networking/socket.h"
//#include "../../shared/networking/packet/packet.h"

#include <exception>         // For exception class

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
        UserSession(uint32 id, TCPSocket* Socket) { m_id = id; m_Socket = Socket; }
        ~UserSession()
        {
            delete m_Socket;
            //while (Packet* packet = _recvQueue.pop_front())
            //    delete packet;
            //while (Packet* packet = _sendQueue.pop_front())
            //    delete packet;
        }

        void UpdatePacket();
        //void QueuePacket(Packet* new_packet);
        //void SendPacket(Packet* packet);
        
        void SetSecurity(uint8 security) { m_security = security; };
        uint8 GetSecurity() const { return m_security; };

        /*

        Funzioni elaborazione pacchetto specifiche sull'utente, tipo cambio livello moderazione ecc..

        */

        uint32 getId() const { return m_id; };

    private:
        uint32 m_id;
        uint8 m_security;
        TCPSocket* m_Socket;
        std::string m_Address;
        
        //list<Packet*> _recvQueue;
        //list<Packet*> _sendQueue;
        
        /// Class used for managing encryption
        //Crypt m_Crypt;
};

#endif  /* _USERSESSION_H */
