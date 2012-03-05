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

SessionManager::SessionManager(): next_id(1), net_number(0), exec_number(0), active_sessions(0)
{
    MutexInit();
}

SessionManager::~SessionManager()
{
    pthread_mutex_destroy(&mutex_sessions);
    pthread_mutex_destroy(&mutex_exec_number);
    pthread_mutex_destroy(&mutex_net_number);
}

void SessionManager::CreateSession (SocketServer* sock)
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
    active_sessions++;
    releaselock_sessions();
}

void SessionManager::DeleteSession (uint32 id)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr != sessions.end())
    {        
        itr->second->getlock_session();
        if (itr->second->IsActive())
        {
            itr->second->ToDelete();
            net_task task;
            task.ptr = (void*)itr->second;
            task.p_pack = NULL;
            task.type_task = KILL;
            n_queue.push(task);
            if (active_sessions > 0)
                active_sessions--;
        }
        itr->second->releaselock_session();
    }
}

void SessionManager::DeleteSession (Session* ses)
{
    ses->getlock_session();
    if (ses->IsActive())
    {
        ses->ToDelete();
        net_task task;
        task.ptr = (void*)ses;
        task.p_pack = NULL;
        task.type_task = KILL;
        n_queue.push(task);
        if (active_sessions > 0)
            active_sessions--;
    }
    ses->releaselock_session();
}

UserSession* SessionManager::GetNextSessionToServe()
{
    UserSession* pUser = NULL;
    if (!sessions.empty())
        if (!IsMoreNetThreadsThanClients())        
            pUser = n_queue.NextUserSessionToServe();        
    return pUser;
}

void SessionManager::AddTaskToServe(net_task* ntask)
{
    n_queue.push(*ntask);  
}

UserSession* SessionManager::GetNextSessionToExecute()
{
    UserSession* pUser = NULL;
    if (!sessions.empty())
        if (!IsMoreExecThreadsThanClients())        
            pUser = e_queue.NextUserSessionToServe();        
    return pUser; 
}

void SessionManager::AddTaskToExecute(exec_task* etask)
{
    e_queue.push(*etask);       
}

void SessionManager::EndSessionServe(uint32 id)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr != sessions.end())
        itr->second->releaselock_net();
}

void SessionManager::EndSessionExecute(uint32 id)
{
    usersession_map::iterator itr = sessions.find(id);
    if (itr != sessions.end())
        itr->second->releaselock_exec();
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
    if (itr->second->IsActive() && (itr->second->GetUserSession()->GetTime() > new_packet->GetTime()))
    {  
        net_task task;
        task.ptr = (void*)itr->second;
        task.p_pack = new_packet;
        task.type_task = SEND;
        n_queue.push(task);        
    }
    else
        delete new_packet;
    itr->second->releaselock_session();
}

void SessionManager::SendPacketTo (UserSession* uses, Packet* new_packet)
{
    uses->getSession()->getlock_session(); 
    if (uses->getSession()->IsActive() && (uses->GetTime() > new_packet->GetTime()))
    {  
        net_task task;
        task.ptr = (void*)uses->getSession();
        task.p_pack = new_packet;
        task.type_task = SEND;
        n_queue.push(task);        
    }
    else
        delete new_packet;
    uses->getSession()->releaselock_session();
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
    bool b_temp = false;
    getlock_sessions();
    if (net_number > active_sessions)
    {
        b_temp = true;
        DecNetThread();
    }
    releaselock_sessions();
    return b_temp;
}

bool SessionManager::IsMoreExecThreadsThanClients()
{
    bool b_temp = false;
    getlock_sessions();
    if (exec_number > active_sessions)
    {
        b_temp = true;
        DecExecThread();
    }
    releaselock_sessions();
    return b_temp;
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
