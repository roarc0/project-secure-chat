#include "session-manager.h"

SessionManager* SessionManager::_instance = NULL;

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

SessionManager::SessionManager(): 
next_id(0), m_sessionLimit(0), net_number(0), exec_number(0)
{

}

SessionManager::~SessionManager()
{
    while (!m_sessions.empty())
    {
        delete m_sessions.begin()->second;
        m_sessions.erase(m_sessions.begin());
    }

    Session* pSes = NULL;
    while (addSessQueue.next(pSes))
        delete pSes;
}

/*
void SessionManager::CreateSession (SocketServer* sock)
{
    Lock guard(mutex_m_sessions);
    UserSession* us = new UserSession(sock);
    SessionMap::iterator itr = m_sessions.begin();
    for (; itr != m_sessions.end(); itr++)
    {
        Lock guard(itr->second->mutex_session);
        if (itr->second->IsFree())
            itr->second->SetSession(us, itr->first);
    }
    if (itr == m_sessions.end())
    {   
        us->SetId(next_id);
        Session* ses = new Session(us);
        m_sessions.insert(usersession_pair(next_id, ses));
        next_id++;
    }
    active_m_sessions++;    
}*/

bool SessionManager::RemoveSession(uint32 id)
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end() && itr->second)
    {
        //if (itr->second->IsLoading())
        //    return false;
        itr->second->KickSession();
    }

    return true;
}

void SessionManager::AddTaskToServe(net_task* ntask)
{
    n_queue.push(*ntask);  
}

void SessionManager::GetIdList(std::list<uint32>* ulist)
{
    SessionMap::iterator itr = m_sessions.begin();

    for(;itr!=m_sessions.end();itr++)
        ulist->push_back(itr->first);
}

uint32 SessionManager::GetUsersessionId(UserSession* usession)
{
    SessionMap::iterator itr = m_sessions.begin();

    for(;itr!=m_sessions.end();itr++)
        if (itr->second->GetUserSession() == usession)
            return itr->first;

    return 0;
}

void SessionManager::SendPacketTo (uint32 id, Packet* new_packet) throw(SessionManagerException)
{
    SessionMap::iterator itr = m_sessions.find(id);
    if (itr == m_sessions.end())
        throw SessionManagerException("Id not found (SendPacketTo(uint32 id, Packet* new_packet))");
    Lock guard(itr->second->mutex_session);
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
}

void SessionManager::SendPacketTo (UserSession* uses, Packet* new_packet)
{
    Lock guard(uses->getSession()->mutex_session);
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
}

std::string SessionManager::GetNameFromId(uint32 id)
{
    SessionMap::iterator itr = m_sessions.find(id);
    if (itr == m_sessions.end())
        throw SessionManagerException("Id not found (GetNameFromId(uint32 id))");
    std::string name = "";
    Lock guard(itr->second->mutex_session);
        if (itr->second->IsActive())
            name = itr->second->GetUserSession()->GetName();
    return name;
}

bool SessionManager::IsMoreNetThreadsThanClients()
{    
    Lock guard(mutex_m_sessions);

    bool b_temp = false;
    if (net_number > active_m_sessions)
    {
        b_temp = true;
        DecNetThread();
    }
    
    return b_temp;
}

bool SessionManager::IsMoreExecThreadsThanClients()
{
    bool b_temp = false;
    Lock guard(mutex_m_sessions);
    if (exec_number > active_m_sessions)
    {
        b_temp = true;
        DecExecThread();
    }
    
    return b_temp;
}

void SessionManager::IncNetThread()
{
    Lock guard(mutex_net_number);

    net_number++;    
}

void SessionManager::DecNetThread()
{
    Lock guard(mutex_net_number);

    if (net_number > 0)
        net_number--;    
}

void SessionManager::IncExecThread()
{
    Lock guard(mutex_exec_number);

    exec_number++;     
}

void SessionManager::DecExecThread()
{
    Lock guard(mutex_exec_number);

    if (exec_number > 0)
        exec_number--;    
}

void SessionManager::AddSession(Socket* sock)
{    
    UserSession* us = new UserSession(sock);
    Session* sess = new Session(us);
    uint32 sessions = GetActiveSessionCount();

    if ((!m_sessionLimit || sessions < m_sessionLimit))
    {
        addSessQueue.add(ses);
    }
    else
        AddQueuedSession(Session* sess)
}

void SessionManager::Update()
{
    AddSessions_ ();

    // Update all sessions
    Session* pSession = NULL;    
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
    {
        pSession = itr->second;
        SingleSessionFilter updater(pSession);
        // If return false we must delete it
        if (!pSession->Update(updater))
        {
            RemoveQueuedSession(pSession);
            m_sessions.erase(itr);
            delete pSession;
        }        
    }
}

uint32 SessionManager::GetQueuePos(Session* sess)
{
    uint32 position = 1;

    for (SessionQueue::const_iterator iter = m_QueuedSessions.begin(); iter != m_QueuedSessions.end(); ++iter, ++position)
        if ((*iter) == sess)
            return position;

    return 0;
}

void SessionManager::AddQueuedSession(Session* sess)
{
    sess->SetInQueue(true);
    m_QueuedSessions.push_back(sess);
}

bool SessionManager::RemoveQueuedSession(Session* sess)
{
    uint32 sessions = GetActiveSessionCount();

    uint32 position = 1;
    Queue::iterator iter = m_waitSessQueue.begin();

    bool found = false;

    for (; iter != m_waitSessQueue.end(); ++iter, ++position)
    {
        if (*iter == sess)
        {
            sess->SetInQueue(false);
            iter = m_waitSessQueue.erase(iter);
            found = true;                                   // removing queued session
            break;
        }
    }

    // if session not queued then we need decrease sessions count
    if (!found && sessions)
        --sessions;

    // accept first in queue
    if ((!m_sessionLimit || sessions < m_sessionLimit) && !m_waitSessQueue.empty())
    {
        WorldSession* pop_sess = m_waitSessQueue.front();
        pop_sess->SetInQueue(false);

        m_waitSessQueue.pop_front();

        iter = m_waitSessQueue.begin();
        position = 1;
    }

    // Update Queue Position
    for (; iter != m_waitSessQueue.end(); ++iter, ++position)
        (*iter)->SendWaitQue(position);

    return found;
}


void SessionManager::AddSession_()
{
    // Add new sessions
    Session* sess = NULL;
    uint32 next_id = 0;

    SessionMap::iterator itr = m_sessions.begin();

    while (addSessQueue.next(sess))
    {
        for (; itr != m_sessions.end(); itr++)
        {
            if (next_id != (itr->first-1))
            {
                us->SetId(next_id);
                m_sessions.insert(usersession_pair(sess->GetId(), sess));
                RemoveQueuedSession(sess);           
                break;
            }
            else
                next_id = itr->first;
        }
    }
}
