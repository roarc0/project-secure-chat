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

class NetQueue : public mt_queue<net_task>
{
    public:
    NetQueue(): mt_queue<net_task>() {};
    Session* NexSessionToServe()
    {
       
    }
};

#endif
