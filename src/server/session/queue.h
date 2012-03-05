#ifndef QUEUE_H
#define QUEUE_H

#include "session.h"
#include "../../shared/utility/mt_queue.h"

enum e_net_task
{
    SEND = 1,
    RECV,
    KILL,
    MAX_NET_TASK
};

struct net_task
{    
    void* ptr;
    Packet* p_pack;
    e_net_task type_task;
};

struct exec_task
{    
    void* ptr;
    Packet* p_pack;
};


class NetQueue : public mt_queue<net_task>
{
    public:
    NetQueue(): mt_queue<net_task>() {};
    UserSession* NextUserSessionToServe()
    {
        get_lock();
        UserSession* uses = NULL;
        Session* ses = NULL;
        std::list<net_task>::iterator itr = m_list.begin();
        for ( ; itr!=m_list.end() ; itr++)
        {
            ses = (Session*)itr->ptr;
            ses->getlock_session();
            if (ses->getlock_net())
                uses = ses->GetUserSession();
            ses->releaselock_session();
        }
        release_lock();
        return uses;
    }
};

class ExecQueue : public mt_queue<exec_task>
{
    public:
    ExecQueue(): mt_queue<exec_task>() {};
    UserSession* NextUserSessionToServe()
    {
        get_lock();
        UserSession* uses = NULL;
        Session* ses = NULL;
        std::list<exec_task>::iterator itr = m_list.begin();
        for ( ; itr!=m_list.end() ; itr++)
        {
            ses = (Session*)itr->ptr;
            ses->getlock_session();
            if (ses->getlock_exec())
                uses = ses->GetUserSession();
            ses->releaselock_session();
        }
        release_lock();
        return uses;
    }
};

#endif
