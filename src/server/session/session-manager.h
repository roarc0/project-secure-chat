#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include <map>
#include <pthread.h>
#include "../../shared/session/user-session.h"

#define s_manager      SessionManager::GetInstance()

typedef std::map<uint32, Session*>  usersession_map;
typedef std::pair<uint32, Session*> usersession_pair;

class Session
{
    public:
        Session(UserSession* pUser)
        {
            pthread_mutex_init(&mutex_session, NULL);
            pthread_mutex_init(&mutex_m_active, NULL);
            pthread_mutex_init(&mutex_net, NULL);
            pthread_mutex_init(&mutex_exec, NULL);
            m_pUser = pUser;
            m_pUser->SetSession(this);
            m_active = 1;
        }

        ~Session()
        {
            pthread_mutex_destroy(&mutex_session);
            pthread_mutex_destroy(&mutex_m_active);
            pthread_mutex_destroy(&mutex_net);
            pthread_mutex_destroy(&mutex_exec);
        }

        // TODO Inserire tempo di creazione della session per controllo di pacchetti precedenti
        void SetSession(UserSession* pUser, uint32 id) { m_pUser = pUser; m_active = 1; m_pUser->SetId(id); m_pUser->SetSession(this);}
        UserSession* GetUserSession() { return m_pUser; }

        bool IsActive() { return m_active == 1 ? true : false; }
        bool IsFree() { return m_active == 0 ? true : false; }
        bool IsToDelete() { return m_active == -1 ? true : false; }
        void Free() 
        {
            delete m_pUser;
            m_pUser = NULL;
            m_active = 0;
            releaselock_exec();
            releaselock_net();
        }
        void ToDelete()
        {
            m_active = -1;
        }
        
        void  getlock_session() { pthread_mutex_lock(&mutex_session); }
        void  releaselock_session() { pthread_mutex_unlock(&mutex_session); }
        void  getlock_active() { pthread_mutex_lock(&mutex_m_active); }
        void  releaselock_actives() { pthread_mutex_unlock(&mutex_m_active); }

        void  releaselock_net() { pthread_mutex_unlock(&mutex_net); }
        // Non bloccante
        bool getlock_net()
        {
            if (!IsActive()){
                return  false;}
            if (pthread_mutex_trylock (&mutex_net) != 0)
                return  false;
            else
                return true;
        }        

        void  releaselock_exec() { pthread_mutex_unlock(&mutex_exec); }
        // Non bloccante
        bool getlock_exec()
        {
            if (IsToDelete())                                           // E' da cancellare
                if (pthread_mutex_trylock (&mutex_exec) == 0)           // Cerco di ottenere il mutex di exec
                    if (pthread_mutex_trylock (&mutex_net) == 0)        // Cerco di ottenere il mutex di net
                    {
                        Free();                                         // Cancello
                        return false;
                    }
                    else
                    {
                        releaselock_exec();                             // Non ho ottenuto il mutex di net, rilascio quello di exec
                        return false;
                    }
                        
           // if (IsFree() || (IsActive() && m_pUser->RecvSize() == 0))     // Non e' valida se e' una sessione libera o se la coda di pacchetti 
           //      return  false;                                         // da servire e' vuota
            if (pthread_mutex_trylock (&mutex_exec) != 0)               // Provo a prendere il mutex di exec
                return  false;
            else
                return true;
        }
        

    private:
        pthread_mutex_t    mutex_session;
        pthread_mutex_t    mutex_m_active;

        pthread_mutex_t    mutex_net;
        pthread_mutex_t    mutex_exec;

        int m_active;
        UserSession* m_pUser;
};

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

        static SessionManager* GetInstance()
        {
            if (!smgr_singleton)
                smgr_singleton = new SessionManager();
            return smgr_singleton;
        };

        ~SessionManager();

        void createSession (TCPSocket* sock);
        void deleteSession (uint32 id);
        
        UserSession* getNextSessionToServe();
        UserSession* getNextSessionToExecute();
        void endSessionServe(uint32 id);
        void endSessionExecute(uint32 id);

        std::string GetNameFromId(uint32 id);
        void        GetIdList(std::list<uint32>*);
        uint32      GetUsersessionId(UserSession*);

        void SendPacketTo(uint32 id, Packet* new_packet) throw(SessionManagerException);

    private:
        usersession_map sessions;
        uint32 next_id;

        usersession_map::iterator it_net;
        usersession_map::iterator it_exec;

        // MUTEX

        pthread_mutex_t           mutex_sessions;

        pthread_mutex_t           mutex_it_net;    // Mutex su usersession_map::iterator it_net
        pthread_mutex_t           mutex_it_exec;   // Mutex su usersession_map::iterator it_exec
        static SessionManager*    smgr_singleton;

        SessionManager();
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
