#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include <map>
#include <pthread.h>
#include "queue.h"
#include "../../shared/threading/mutex.h"
#include "../../shared/utility/singleton.h"
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
class SessionManager : public Singleton
{
    public:

        ~SessionManager();

        void CreateSession (SocketServer* sock);
        void DeleteSession (uint32 id);
        void DeleteSession (Session* ses);

        void AddTaskToServe(net_task* ntask);
        void AddTaskToExecute(exec_task* etask);        
        UserSession* GetNextSessionToServe();
        UserSession* GetNextSessionToExecute();
        void EndSessionServe(uint32 id);
        void EndSessionExecute(uint32 id);

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

    private:
        usersession_map sessions;
        
        NetQueue n_queue;
        ExecQueue e_queue;

        uint32 next_id;
        uint32 active_sessions;
        uint32 net_number;
        uint32 exec_number;

        // MUTEX

        Mutex   mutex_sessions;

        Mutex   mutex_net_number;    // Mutex su net_number
        Mutex   mutex_exec_number;   // Mutex su exec_number

        SessionManager();
};

#endif  /* _SESSION_MRG_H */
