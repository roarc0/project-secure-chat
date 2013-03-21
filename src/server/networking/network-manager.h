#ifndef _NETWORK_MNG_H
#define _NETWORK_MNG_H

#include "scheduling-engine.h"
#include "../../shared/common.h"
#include "method-request.h"
#include "session.h"

enum eNetworkType
{
    SEND   = 0,
    RECIVE = 1,
};

typedef std::pair<Session_smart, eNetworkType> netsession_pair;

class NetworkManager
{
    public:
        NetworkManager(uint32 n_thread);
        ~NetworkManager() {}

        int ActivateEpoll();
        int ActivateThreadsNetwork();
        int QueueSend(Session_smart m_ses);
        int QueueRecive(Session_smart m_ses);
        netsession_pair GetNextSession();

    private:
        SchedulingEngine net_engine;
        LockedQueue<netsession_pair> q_request;

        Mutex m_mutex;
        Semaphore sem;
        uint32 m_thread;
};

#endif
