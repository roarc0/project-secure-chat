#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include <map>
#include <pthread.h>
#include "queue.h"
#include "../../shared/threading/mutex.h"
#include "../../shared/utility/singleton.h"
#include "../../shared/utility/packetfilter.h"
#include "../../shared/utility/lockqueue.h"
//#include "../../shared/session/user-session.h"

#define s_manager      SessionManager::GetInstance()

typedef std::map<uint32, Session*>  SessionMap;
typedef std::pair<uint32, Session*> usersession_pair;
typedef std::list<Session*> SessionQueue;

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
class SessionManager : public Singleton
{    
    public:

        ~SessionManager();
        void Update();

        void AddSession(Socket* sock);
        void RemoveSession (uint32 id);

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
        void SendPacketTo(UserSession* uses, Packet* new_packet);

        uint32 GetActiveSessionCount() const { return m_sessions.size() - m_waitSessQueue.size(); }
        uint32 GetQueuedSessionCount() const { return m_waitSessQueue.size(); }
        uint32 GetQueuePos(Session* sess);
        void   AddQueuedSession(Session* sess);

    private:

        void AddSession_();

        SessionMap m_sessions;

        // Sessioni in attesa di essere aggiunte alla m_sessions
        LockedQueue<Session*> addSessQueue;

        // Coda in Ingresso
        SessionQueue m_QueuedSessions;

        uint32 m_sessionLimit;
        
        NetQueue n_queue;
        ExecQueue e_queue;

        uint32 net_number;
        uint32 exec_number;        

        // MUTEX
        Mutex   mutex_net_number;    // Mutex su net_number
        Mutex   mutex_exec_number;   // Mutex su exec_number

        SessionManager();
};

#endif  /* _SESSION_MRG_H */
