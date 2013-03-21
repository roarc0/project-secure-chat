#ifndef _NETWORK_MNG_H
#define _NETWORK_MNG_H

#include "scheduling-engine.h"
#include "../../shared/common.h"
#include "method-request.h"
#include "session.h"

class NetworkManager
{
    public:
        NetworkManager();
        ~NetworkManager() {}

        int Queue(Session* m_ses);
        Session* GetNextSession();

    private:
        SchedulingEngine net_engine;
        LockedQueue<Session*> q_request;

        Mutex m_mutex;
        Semaphore sem;
};

#endif
