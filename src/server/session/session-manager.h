#ifndef _SESSION_MRG_H
#define _SESSION_MRG_H

#include "session.h"
#include <map>
#include <pthread.h>

class Session
{
    public:
        Session(UserSession* pUser)
        {
            m_active = 0;
            pthread_mutex_init(&mutex_session, NULL);
            pthread_mutex_init(&mutex_m_active, NULL);
            pthread_mutex_init(&mutex_net, NULL);
            pthread_mutex_init(&mutex_exec, NULL);
        }

        // TODO Inserire tempo di creazione della session per controllo di pacchetti precedenti
        void SetSession(UserSession* ses) { pUser = ses; m_active = 1;}
        UserSession* GetUserSession() { return pUser; }

        bool IsActive()
        {
            return m_active == 1 ? true : false;
        }     

        bool IsFree()
        {
            return m_active == 0 ? true : false;
        }

        bool IsToDelete()
        {
            return m_active == -1 ? true : false;
        }

        void ToDelete()
        {
            m_active = -1;
        }
        
        void  getlock_session() { pthread_mutex_lock(&mutex_session); }
        void  releaselock_session() { pthread_mutex_unlock(&mutex_session); }
        void  getlock_active() { pthread_mutex_lock(&mutex_m_active); }
        void  releaselock_actives() { pthread_mutex_unlock(&mutex_m_active); }

        // Non bloccante
        bool getlock_net()
        {
            if (pthread_mutex_trylock (&mutex_net) != 0)
                return  false;
            else
                return true;
        }
        void  releaselock_net() { pthread_mutex_unlock(&mutex_net); }

        // Non bloccante
        bool getlock_exec()
        {
            if (IsActive() && pUser->RecvSize() == 0)
                return  false;
            if (pthread_mutex_trylock (&mutex_exec) != 0)
                return  false;
            else
                return true;
        }
        void  releaselock_exec() { pthread_mutex_unlock(&mutex_exec); }

    private:
        pthread_mutex_t    mutex_session;
        pthread_mutex_t    mutex_m_active;

        pthread_mutex_t    mutex_net;
        pthread_mutex_t    mutex_exec;

        int m_active;
        UserSession* pUser;     
};

typedef std::map<uint32, Session*>  usersession_map;
typedef std::pair<uint32, Session*> usersession_pair;

class SessionManagerException : public exception 
{
	public:
		SessionManagerException(const std::string &message, bool inclSysMsg = false) throw();
		~SessionManagerException() throw();

		const char *what() const throw();

	private:
		std::string userMessage;  // Exception message
};

// Classe di gestione delle sessioni aperte
class SessionManager
{
    public:
	    SessionManager();
        ~SessionManager();

        void addSession (UserSession* us);
        void deleteSession (uint32 id);        
        
        UserSession* getNextSessionToServe();
        UserSession* getNextSessionToExecute();

        void SendPacketTo(uint32 id, Packet* new_packet) throw(SessionManagerException);
        
    private:
        usersession_map sessions;

	    usersession_map::iterator it_net;
        usersession_map::iterator it_exec;

        // MUTEX

        pthread_mutex_t    mutex_sessions;

        pthread_mutex_t    mutex_it_net;
        pthread_mutex_t    mutex_it_exec;        

        inline void  MutexInit()
        {
            pthread_mutex_init(&mutex_sessions, NULL);
            pthread_mutex_init(&mutex_it_net, NULL);
            pthread_mutex_init(&mutex_it_exec, NULL);
        }

        inline void  getlock_sessions() { pthread_mutex_lock(&mutex_sessions); }
        inline void  releaselock_sessions() { pthread_mutex_unlock(&mutex_sessions); }

        inline void  getlock_it_net() { pthread_mutex_lock(&mutex_it_net); }
        inline void  releaselock_it_net() { pthread_mutex_unlock(&mutex_it_net); }  
  
        inline void  getlock_it_exec() { pthread_mutex_lock(&mutex_it_exec); }
        inline void  releaselock_it_exec() { pthread_mutex_unlock(&mutex_it_exec); }  
};

#endif  /* _SESSION_MRG_H */
