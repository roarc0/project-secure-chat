#ifndef QUEUE_H
#define QUEUE_H

#include "session.h"
#include "../../shared/utility/mt_queue.h"

class Packet;

enum e_net_task
{
    SEND = 1,
    RECV,
    NEW,
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
        Lock guard(_mutex);
        UserSession* uses = NULL;
        Session* ses = NULL;
        std::list<net_task>::iterator itr = _list.begin();
        for ( ; itr!=_list.end() ; itr++)
        {
            ses = (Session*)itr->ptr; 
            ses->getlock_session();         
            if (itr->type_task == KILL)
            {
                if (!ses->TryDelete())                
                    push_l(*itr);
                erase_l(itr);
                ses->releaselock_session();
                break;
            }
            switch (ses->getlock_net())
            {
                case 1: 
                    uses = ses->GetUserSession();
                    break;
                default:
                    erase_l(itr);
                    break;
            }
            ses->releaselock_session();
        }
        return uses;
    }
};

class ExecQueue : public mt_queue<exec_task>
{
    public:
    ExecQueue(): mt_queue<exec_task>() {};
    UserSession* NextUserSessionToServe()
    {
        Lock guard(_mutex);            
        UserSession* uses = NULL;
        Session* ses = NULL;
        std::list<exec_task>::iterator itr = _list.begin();
        for ( ; itr!=_list.end() ; itr++)
        {
            ses = (Session*)itr->ptr;
            ses->getlock_session();
            if (ses->getlock_exec())
            {
                uses = ses->GetUserSession();
                erase_l(itr);  
            }
            ses->releaselock_session();
        }
        return uses;
    }
};

#endif
