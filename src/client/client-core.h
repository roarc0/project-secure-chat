#ifndef CORE_CLIENT_H
#define CORE_CLIENT_H

#include "common.h"
#include "utility/singleton.h"
#include "threading/thread.h"
#include "session/session.h"

void* CoreThread(void*);

class ClientCore
{
    Session*       session;
    pthread_cond_t cond_connection;
    pthread_mutex_t mutex_connection;

    ClientCore();

  public:

    ~ClientCore();

    bool  Connect();
    bool  Disconnect();
    bool  HandleSend(const char*);
    void  HandleRecv();

    void  GuiUpdateStatusBar(const char*);
    int   StartThread(Session *sc);

    void WaitConnection()
    {
        pthread_cond_wait(&cond_connection, &mutex_connection);
    }

    void SignalConnection()
    {
        pthread_cond_signal(&cond_connection);
    }

    bool  IsConnected()
    {
        return session->IsConnected();
    }

    friend class Singleton<ClientCore>;
};

#define c_core  Singleton<ClientCore>::GetInstance()

#endif
