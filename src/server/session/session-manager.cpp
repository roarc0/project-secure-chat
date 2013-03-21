#include "session-manager.h"

SessionManager::SessionManager(): 
m_sessionLimit(0), m_sessionActiveLimit(0), net_number(0), exec_number(0)
{
    channelMrg = new ChannelManager();
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

    delete channelMrg;
}

void SessionManager::GetIdList(std::list<uint32>* ulist)
{
    SessionMap::iterator itr = m_sessions.begin();

    for(;itr!=m_sessions.end();itr++)
        ulist->push_back(itr->first);
}

int SessionManager::AddSession(SocketBase* sock)
{
    if (GetQueuedSessionCount() + addSessQueue.size() <  m_sessionLimit)
    {
        Session* ses = new Session(sock);
        addSessQueue.add(ses);
    }
    else
    {
        // Disconetti sessione
    }
    return 0;
}

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

/// Find a session by its id
Session* SessionManager::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end())
        return itr->second;
    else
        return NULL;
}

void SessionManager::Update(uint32 udiff)
{
    AddSessions_ ();

    // Update all sessions
    Session* pSession = NULL;    
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
    {
        pSession = itr->second;
        SingleSessionFilter updater(pSession);
        // If return false we must delete it
        if (!pSession->Update(udiff, updater))
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

    // TODO Notifica all'utente che è in attesa, GetQueuePos(sess)
}

bool SessionManager::RemoveQueuedSession(Session* sess)
{
    uint32 sessions = GetActiveSessionCount();

    uint32 position = 1;
    SessionQueue::iterator iter = m_waitSessQueue.begin();

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
    if ((!m_sessionActiveLimit || sessions < m_sessionActiveLimit) && !m_waitSessQueue.empty())
    {
        Session* pop_sess = m_waitSessQueue.front();
        pop_sess->SetInQueue(false);

        // TODO notifica all'utente che è stato accettato

        m_waitSessQueue.pop_front();

        iter = m_waitSessQueue.begin();
        position = 1;
    }

    // Update Queue Position
    for (; iter != m_waitSessQueue.end(); ++iter, ++position)
        (*iter)->SendWaitQueue(position);

    return found;
}


void SessionManager::AddSessions_()
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
                AddSession_(next_id, sess);
                next_id++;                    
                break;
            }
            else
                next_id = itr->first;
        }
    }
}

void SessionManager::AddSession_(uint32& next_id, Session* sess)
{
    if (m_sessionActiveLimit && GetActiveSessionCount() >= m_sessionActiveLimit)
    {
        AddQueuedSession(sess);
    }

    sess->SetId(next_id);
    m_sessions.insert(usersession_pair(next_id, sess));
}
