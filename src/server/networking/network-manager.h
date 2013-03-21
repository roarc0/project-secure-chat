#ifndef _NETWORK_MNG_H
#define _NETWORK_MNG_H

#include "scheduling-engine.h"
#include "../../shared/common.h"
#include "method-request.h"
#include "session.h"

class NetworkManager
{
    public:
        NetworkManager(uint32 n_thread);
        ~NetworkManager() {}

        int ActivateEpoll();
        int ActivateThreadsNetwork();
        int Queue(Session* m_ses);
        Session* GetNextSession();

    private:
        SchedulingEngine net_engine;
        LockedQueue<Session*> q_request;

        Mutex m_mutex;
        Semaphore sem;
        uint32 m_thread;
};

#endif
