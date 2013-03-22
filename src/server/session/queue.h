#ifndef QUEUE_H
#define QUEUE_H

#include "session.h"
#include "../../shared/utility/queues/mt_queue.h"

class Packet;

struct net_task
{    
    void* ptr;
};

class NetQueue : public mt_queue<net_task>
{
    public:
    NetQueue(): mt_queue<net_task>() {};
    Session* NexSessionToServe()
    {
       return NULL;
    }
};

#endif
