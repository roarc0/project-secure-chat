#include "sessionsmanager.h"

void SessionsManager::addSession (const UserSession& us) 
{
   sessions.insert(make_pair(us.getId(), us));
}
void SessionsManager::deleteSession (uint32 id)
{
    sessions.erase(id);
}

		
UserSession& SessionsManager::getNextSessionToServe()
{
    // used by network threads: scrolls sessions, gets priority and mutex on the first one not locked by other network threads 
    return sessions.begin()->second;
}
UserSession& SessionsManager::getNextSessionToExecute()
{
    // used by execute threads: scrolls sessions, gets priority and mutex on the first one not locked by other execute threads 
    return sessions.begin()->second;
}
