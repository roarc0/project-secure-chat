#include "session-manager.h"

SessionManager* SessionManager::smgr_singleton = NULL;

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
    net_number = 0;
    exec_number = 0;

    MutexInit();
}

SessionManager::~SessionManager()
{
    pthread_mutex_destroy(&mutex_sessions);
    pthread_mutex_destroy(&mutex_it_net);
    pthread_mutex_destroy(&mutex_it_exec);
    pthread_mutex_destroy(&mutex_exec_number);
    pthread_mutex_destroy(&mutex_net_number);
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
        us->SetId(next_id);
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
    if (!IsMoreNetThreadsThanClients())
        if (!sessions.empty())
        {
            if (it_net == sessions.end())
                it_net = sessions.begin();
            while (!pUser)
            {
                it_net->second->getlock_session();
                if (it_net->second->getlock_net())
                    pUser = it_net->second->GetUserSession();
                it_net->second->releaselock_session();
                it_net++;
                if (it_net == sessions.end())
                    it_net = sessions.begin();
            } 
        }
    releaselock_it_net(); // End Mutex
    return pUser;         
}

UserSession* SessionManager::getNextSessionToExecute()
{
    UserSession* pUser = NULL;
    getlock_it_exec(); // Get Mutex
    if (!IsMoreExecThreadsThanClients())
        if (!sessions.empty())
        {
            if (it_exec == sessions.end())
                it_exec = sessions.begin();
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

void SessionManager::GetIdList(std::list<uint32>* ulist)
{
    usersession_map::iterator itr = sessions.begin();

    for(;itr!=sessions.end();itr++)
        ulist->push_back(itr->first);
}

uint32 SessionManager::GetUsersessionId(UserSession* usession)
{
    usersession_map::iterator itr = sessions.begin();

    for(;itr!=sessions.end();itr++)
        if (itr->second->GetUserSession() == usession)
            return itr->first;

    return 0;
}

void SessionManager::SendPacketTo (uint32 id, Packet* new_packet) throw(SessionManagerException)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr == sessions.end())
        throw SessionManagerException("Id not found (SendPacketTo(uint32 id, Packet* new_packet))");
    itr->second->getlock_session();

    // per ora voglio vedere se arriva
    //if (itr->second->IsActive() && (itr->second->GetUserSession()->GetTime() > new_packet->GetTime()))
        itr->second->GetUserSession()->QueuePacketToSend(new_packet);
    //else  // il delete va fatto quando possibile nella funzione che ha fatto la new
    //    delete new_packet;
    itr->second->releaselock_session();
}

std::string SessionManager::GetNameFromId(uint32 id)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr == sessions.end())
        throw SessionManagerException("Id not found (GetNameFromId(uint32 id))");
    std::string name = "";
    itr->second->getlock_session();
        if (itr->second->IsActive())
            name = itr->second->GetUserSession()->GetName();
    itr->second->releaselock_session();
    return name;
}

bool SessionManager::IsMoreNetThreadsThanClients()
{
    return false;
}

bool SessionManager::IsMoreExecThreadsThanClients()
{
    return false;
}

void SessionManager::IncNetThread()
{
    getlock_net_number();
    net_number++;
    releaselock_net_number();    
}

void SessionManager::DecNetThread()
{
    getlock_net_number();
    if (net_number > 0)
        net_number--;
    releaselock_net_number();    
}

void SessionManager::IncExecThread()
{
    getlock_exec_number();
    exec_number++;
    releaselock_exec_number();    
}

void SessionManager::DecExecThread()
{
    getlock_exec_number();
    if (exec_number > 0)
        exec_number--;
    releaselock_exec_number();    
}
