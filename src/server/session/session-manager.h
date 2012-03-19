#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include <map>
#include <pthread.h>
#include "queue.h"
#include "../../shared/common.h"
#include "../../shared/threading/mutex.h"
#include "../../shared/singleton.h"
#include "../../shared/networking/packetfilter.h"
#include "../../shared/queues/lock_queue.h"

typedef UNORDERED_MAP<uint32, Session*>  SessionMap;
typedef std::pair<uint32, Session*> usersession_pair;
typedef std::list<Session*> SessionQueue;

NEWEXCEPTION(SessionManagerException);

// Classe di gestione delle sessioni aperte
class SessionManager
{    
        friend class Singleton<SessionManager>;

    public:

        ~SessionManager();

        void AddTaskToServe(void* ptr)
        {
            n_queue.push(*((net_task*)ptr));
        }

        void GetIdList(std::list<uint32>*);

        int AddSession(Socket* sock);
        bool RemoveSession (uint32 id);
        Session* FindSession(uint32 id) const;

        void Update(uint32 udiff);

        uint32 GetActiveSessionCount() const { return m_sessions.size() - m_waitSessQueue.size(); }
        uint32 GetSessionCount()       const { return m_sessions.size(); }
        uint32 GetQueuedSessionCount() const { return m_waitSessQueue.size(); }
        uint32 GetQueuePos(Session* sess);

        // Session server limit
        void SetSessionAmountLimit(uint32 limit) { m_sessionLimit = limit; }
        uint32 GetSessionAmountLimit() const { return m_sessionLimit; }

        // Active session server limit
        void SetSessionActiveAmountLimit(uint32 limit) { m_sessionActiveLimit = limit; }
        uint32 GetSessionActiveAmountLimit() const { return m_sessionActiveLimit; }

    private:
        void AddQueuedSession(Session* sess);
        bool RemoveQueuedSession(Session* sess);
        void AddSessions_();
        void AddSession_(uint32& next_id, Session* sess);

        // Sessions Map
        SessionMap m_sessions;

        // Sessioni in attesa di essere aggiunte alla m_sessions
        LockedQueue<Session*> addSessQueue;

        // Coda in Ingresso
        SessionQueue m_QueuedSessions;
        SessionQueue m_waitSessQueue;

        // Session server limit
        uint32 m_sessionLimit;
        // // Active session server limit
        uint32 m_sessionActiveLimit;        
        
        NetQueue n_queue;

        uint32 net_number;
        uint32 exec_number;        

        // MUTEX
        Mutex   mutex_net_number;    // Mutex su net_number
        Mutex   mutex_exec_number;   // Mutex su exec_number

        SessionManager();
};

#define s_manager Singleton<SessionManager>::GetInstance()

#endif  /* _SESSION_MRG_H */
