#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include <map>
#include <pthread.h>
#include "queue.h"
//#include "../../shared/session/user-session.h"

#define s_manager      SessionManager::GetInstance()

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

        static SessionManager* GetInstance()
        {
            if (!smgr_singleton)
                smgr_singleton = new SessionManager();
            return smgr_singleton;
        };

        ~SessionManager();

        void createSession (SocketServer* sock);
        void deleteSession (uint32 id);

        void addTaskToServe(net_task* ntask);
        void addTaskToExecute(exec_task* etask);        
        UserSession* getNextSessionToServe();
        UserSession* getNextSessionToExecute();
        void endSessionServe(uint32 id);
        void endSessionExecute(uint32 id);

        std::string GetNameFromId(uint32 id);
        void        GetIdList(std::list<uint32>*);
        uint32      GetUsersessionId(UserSession*);

        void IncNetThread();
        void IncExecThread();
        void DecNetThread();
        void DecExecThread();

        bool IsMoreNetThreadsThanClients();
        bool IsMoreExecThreadsThanClients();

        void SendPacketTo(uint32 id, Packet* new_packet) throw(SessionManagerException);

    private:
        usersession_map sessions;

        NetQueue n_queue;
        ExecQueue e_queue;

        uint32 next_id;
        uint32 active_sessions;
        uint32 net_number;
        uint32 exec_number;

        // MUTEX

        pthread_mutex_t           mutex_sessions;

        pthread_mutex_t           mutex_net_number;    // Mutex su net_number
        pthread_mutex_t           mutex_exec_number;   // Mutex su exec_number
        static SessionManager*    smgr_singleton;

        SessionManager();
        inline void  MutexInit()
        {
            pthread_mutex_init(&mutex_sessions, NULL);
            pthread_mutex_init(&mutex_exec_number, NULL);
            pthread_mutex_init(&mutex_net_number, NULL);            
        }

        inline void  getlock_sessions() { pthread_mutex_lock(&mutex_sessions); }
        inline void  releaselock_sessions() { pthread_mutex_unlock(&mutex_sessions); }
  
        inline void  getlock_net_number() { pthread_mutex_lock(&mutex_net_number); }
        inline void  releaselock_net_number() { pthread_mutex_unlock(&mutex_net_number); }
        inline void  getlock_exec_number() { pthread_mutex_lock(&mutex_exec_number); }
        inline void  releaselock_exec_number() { pthread_mutex_unlock(&mutex_exec_number); } 
};

#endif  /* _SESSION_MRG_H */
