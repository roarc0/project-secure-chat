#include "session-manager.h"

SessionManager::SessionManager()
{
    it_net = sessions.begin();
    it_exec = sessions.begin();

    MutexInit();
}

SessionManager::~SessionManager()
{
}

void SessionManager::addSession (UserSession* us) 
{
    getlock_sessions();
    sessions.insert(usersession_pair(us->getId(), us));
    releaselock_sessions();
}

void SessionManager::deleteSession (uint32 id)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr != sessions.end())
    {
        getlock_sessions();
        getlock_it_net();
        getlock_it_exec();

        if (itr == it_exec)
        {
            it_exec++;
            if (it_exec == sessions.end())
                it_exec = sessions.begin();
        }
        if (itr == it_net)
        {
            it_net++;
            if (it_net == sessions.end())
                it_net = sessions.begin();
        }
        sessions.erase(itr);

        releaselock_it_exec();
        releaselock_it_net();
        releaselock_sessions();
    }
}

UserSession* SessionManager::getNextSessionToServe()
{
    UserSession* pUser = NULL;
    getlock_it_net(); // Get Mutex
    if (it_net != sessions.end())
    {
        pUser = it_net->second;
        it_net++;
    }
    if (it_net == sessions.end())
        it_net = sessions.begin();
    releaselock_it_net(); // End Mutex
    return pUser;         
}

UserSession* SessionManager::getNextSessionToExecute()
{
    UserSession* pUser = NULL;
    getlock_it_exec(); // Get Mutex
    if (it_exec != sessions.end())
    {
        pUser = it_exec->second;
        it_exec++;
    }
    if (it_exec == sessions.end())
        it_exec = sessions.begin();
    releaselock_it_exec(); // End Mutex
    return pUser; 
}
