#include "session-manager.h"

// SocketException Code
SessionManagerException::SessionManagerException (const string &message, bool inclSysMsg)
  throw() : userMessage(message) 
{
	if (inclSysMsg) 
	{
		userMessage.append(": ");
		userMessage.append(strerror(errno));
	}
}

SessionManagerException::~SessionManagerException() throw() 
{

}

const char *SessionManagerException::what() const throw() 
{
	return userMessage.c_str();
}

SessionManager::SessionManager()
{
    it_net = sessions.begin();
    it_exec = sessions.begin();

    next_id = 1;

    MutexInit();
}

SessionManager::~SessionManager()
{
    pthread_mutex_destroy(&mutex_sessions);
    pthread_mutex_destroy(&mutex_it_net);
    pthread_mutex_destroy(&mutex_it_exec);
}

void SessionManager::createSession (TCPSocket* sock) 
{    
    getlock_sessions();
    UserSession* us = new UserSession(sock);
    usersession_map::iterator itr = sessions.begin();
    for (; itr != sessions.end(); itr++)
    {
        itr->second->getlock_session();
        if (itr->second->IsFree())
            itr->second->SetSession(us, itr->first);
        itr->second->releaselock_session();
    }
    if (itr == sessions.end())
    {   
        us->setId(next_id);
        Session* ses = new Session(us);
        sessions.insert(usersession_pair(next_id, ses));
        next_id++;
    }
    releaselock_sessions();
}

void SessionManager::deleteSession (uint32 id)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr != sessions.end())
    {        
        itr->second->getlock_session();
        if (itr->second->IsActive())
            itr->second->ToDelete();
        itr->second->releaselock_session();
    }
}

UserSession* SessionManager::getNextSessionToServe()
{
    UserSession* pUser = NULL;
    getlock_it_net(); // Get Mutex
    while (!pUser)
    {
        it_exec->second->getlock_session();
        if (it_exec->second->getlock_net())
            pUser = it_exec->second->GetUserSession();
        it_exec->second->releaselock_session();
        it_net++;
        if (it_net == sessions.end())
            it_net = sessions.begin();
    } 
    releaselock_it_net(); // End Mutex
    return pUser;         
}

UserSession* SessionManager::getNextSessionToExecute()
{
    UserSession* pUser = NULL;
    getlock_it_exec(); // Get Mutex
    while (!pUser)
    {
        it_exec->second->getlock_session();
        if (it_exec->second->getlock_exec())
            pUser = it_exec->second->GetUserSession();
        it_exec->second->releaselock_session();
        it_exec++;
        if (it_exec == sessions.end())
            it_exec = sessions.begin();
        
    }    
    releaselock_it_exec(); // End Mutex
    return pUser; 
}

void SessionManager::endSessionServe(uint32 id)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr != sessions.end())
    {
        itr->second->getlock_session();
        itr->second->releaselock_net();
        itr->second->releaselock_session();
    }
}

void SessionManager::endSessionExecute(uint32 id)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr != sessions.end())
    {
        itr->second->getlock_session();
        itr->second->releaselock_exec();
        itr->second->releaselock_session();
    }
}

void SessionManager::SendPacketTo (uint32 id, Packet* new_packet) throw(SessionManagerException)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr == sessions.end())
        throw SessionManagerException("Id not found (SendPacketTo(uint32 id, Packet* new_packet))");
    itr->second->getlock_session();
    if (itr->second->IsActive() /* && controllo temporizzazione pacchetto rispetto alla session*/)
        itr->second->GetUserSession()->QueuePacketToSend(new_packet);
    else
        delete new_packet;
    itr->second->releaselock_session();
}
