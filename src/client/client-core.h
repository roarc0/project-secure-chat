#ifndef CORE_CLIENT_H
#define CORE_CLIENT_H

#include "common.h"
#include "utility/singleton.h"
#include "threading/thread.h"
#include "session/session.h"
//#include "networking/packet.h"
//#include "networking/socket-client.h"
void* CoreThread(void*);

class ClientCore
{
    Session*   session;

    ClientCore();

  public:

    ~ClientCore();

    bool  Connect();
    bool  Disconnect();
    bool  HandleSend(const char*);
    void  HandleRecv();

    void  GuiUpdateStatusBar(const char*);
    int   StartThread(Session *sc);

    bool  IsConnected()
    {
        return session->IsConnected();
    }

    friend class Singleton<ClientCore>;
};

#define c_core  Singleton<ClientCore>::GetInstance()

#endif
