#ifndef _SESSION_MRG_H
#define _SESSION_MRG_H

#include "session.h"
#include <map>
#include <pthread.h>

typedef std::map<uint32, UserSession*>  usersession_map;
typedef std::pair<uint32, UserSession*> usersession_pair;

// Classe di gestione delle sessioni aperte
class SessionManager
{
    public:
	    SessionManager();
        ~SessionManager();

        void addSession (UserSession*);
        void deleteSession (uint32 id);
        
        
        UserSession* getNextSessionToServe();
        UserSession* getNextSessionToExecute();
        
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
