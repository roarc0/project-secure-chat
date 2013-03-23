#ifndef CORE_CLIENT_H
#define CORE_CLIENT_H

#include "common.h"
#include "utility/singleton.h"
#include "threading/thread.h"
#include "networking/packet.h"
#include "networking/socket-client.h"

void* CoreThread(void*);

class ClientCore //: public Thread
{
    SocketClient*   csock;
    bool            connected;

    ClientCore();

  public:

    friend class Singleton<ClientCore>;
    ~ClientCore();
    bool  Connect();
    bool  Disconnect();
    bool  HandleSend(const char*);
    void  HandleRecv();

    void  SendPacket(Packet* new_packet);    
    void  GuiUpdateStatusBar(const char*);
    
    bool IsConnected() const
    {
        return connected;
    }

    void SetConnected(bool c)
    {
        connected = c;
    }
    
    int StartThread(SocketClient *sc);

  private:
    int _SendPacket(const Packet& pct);
};

#define c_core  Singleton<ClientCore>::GetInstance()

#endif
