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
        // delete m_sessions.begin()->second; // It's Smart :D
        m_sessions.erase(m_sessions.begin());
    }

    //Session_smart pSes;
    //while (addSessQueue.next(pSes))
    //   delete pSes;

    delete channelMrg;
}

void SessionManager::GetIdList(std::list<uint32>* ulist)
{
    SessionMap::iterator itr = m_sessions.begin();

    for(;itr!=m_sessions.end();itr++)
        ulist->push_back(itr->first);
}

Session_smart SessionManager::AddSession(int sock)
{
    if (GetQueuedSessionCount() + addSessQueue.size() <  m_sessionLimit)
    {        
        Session* ses = new Session(sock);
        assert(ses);
        counted_ptr<Session> smart_ses(ses);
        addSessQueue.add(smart_ses);
    }
    else
    {
        // Disconetti sessione
    }
}

bool SessionManager::RemoveSession(uint32 id)
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end() && itr->second.get())
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
        return itr->second.get();
    else
        return NULL;
}

void SessionManager::Update(uint32 udiff)
{
    AddSessions_ ();

    // Update all sessions
    Session* pSession;    
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
    {
        pSession = itr->second.get();
        SingleSessionFilter updater(pSession);
        // If return false we must delete it
        if (!pSession->Update(udiff, updater))
        {
            RemoveQueuedSession(itr->second);
            m_sessions.erase(itr);
            // delete pSession;  // Is Smart :P
        }        
    }

    channelMrg->Update(udiff);
}

uint32 SessionManager::GetQueuePos(Session_smart sess)
{
    uint32 position = 1;

    for (SessionQueue::const_iterator iter = m_QueuedSessions.begin(); iter != m_QueuedSessions.end(); ++iter, ++position)
        if ((*iter).get() == sess.get())
            return position;

    return 0;
}

void SessionManager::AddQueuedSession(Session_smart sess)
{
    sess->SetInQueue(true);
    m_QueuedSessions.push_back(sess);

    // TODO Notifica all'utente che è in attesa, GetQueuePos(sess)
}

bool SessionManager::RemoveQueuedSession(Session_smart sess)
{
    uint32 sessions = GetActiveSessionCount();

    uint32 position = 1;
    SessionQueue::iterator iter = m_waitSessQueue.begin();

    bool found = false;

    for (; iter != m_waitSessQueue.end(); ++iter, ++position)
    {
        if ((*iter).get() == sess.get())
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
        Session_smart pop_sess = m_waitSessQueue.front();
        pop_sess->SetInQueue(false);

        // TODO notifica all'utente che è stato accettato

        m_waitSessQueue.pop_front();

        iter = m_waitSessQueue.begin();
        position = 1;
    }

    // Update Queue Position
    for (; iter != m_waitSessQueue.end(); ++iter, ++position)
        (*iter).get()->SendWaitQueue(position);

    return found;
}


void SessionManager::AddSessions_()
{
    // Add new sessions
    Session_smart sess;
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

void SessionManager::AddSession_(uint32& next_id, Session_smart sess)
{
    if (m_sessionActiveLimit && GetActiveSessionCount() >= m_sessionActiveLimit)
    {
        AddQueuedSession(sess);
    }

    sess.get()->SetId(next_id);
    m_sessions.insert(usersession_pair(next_id, sess));
}
