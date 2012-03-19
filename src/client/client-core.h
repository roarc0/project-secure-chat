#ifndef CORE_CLIENT_H
#define CORE_CLIENT_H

#include "../shared/common.h"
#include "../shared/singleton.h"
#include "../shared/threading/thread.h"
#include "../shared/execution/command-manager.h"
#include "networking/socket-client.h"

void* CoreThread(void*);

class ClientCore //: public Thread
{
    SocketClient*         csock;
    bool                  connected;

    ClientCore();

  public:

    friend class Singleton<ClientCore>;

    ~ClientCore();

    bool  Connect();
    bool  Disconnect();
    void  HandleSend(const char*);
    void  HandleRecv();

    void  GuiUpdateStatusBar(const char*);
    
    bool IsConnected() const
    {
        return connected;
    }

    void SetConnected(bool c)
    {
        connected = c;
    }

};

#define c_core       Singleton<ClientCore>::GetInstance()

#endif
