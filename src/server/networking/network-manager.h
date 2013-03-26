#ifndef _NETWORK_MNG_H
#define _NETWORK_MNG_H

#include "scheduling-engine.h"
#include "utility/singleton.h"
#include "socket/socket-server.h"
#include "method-request.h"
#include "session.h"
#include "common.h"

enum eNetworkType
{
    SEND   = 0,
    RECV   = 1,
};

typedef std::pair<Session_smart, eNetworkType> netsession_pair;

class NetworkManager
{
    friend class Singleton<NetworkManager>;

    public:
        NetworkManager();
        ~NetworkManager() {}

        int Initialize(uint32 n_thread);
        int ActivateEpoll();
        int ActivateThreadsNetwork();
        int QueueSend(Session_smart m_ses);
        int QueueRecive(uint32 sock);
        netsession_pair GetNextSession();

    private:
        SchedulingEngine net_engine;
        LockQueue<netsession_pair> q_request;

        Mutex m_mutex;
        Semaphore sem;
        uint32 m_thread;
};

#define net_manager Singleton<NetworkManager>::GetInstance()

#endif
