#ifndef _SESSION_H
#define _SESSION_H

#include "session/sessionbase.h"
#include "../networking/socket-client.h"

class Session : public SessionBase
{
    SocketClient* c_Socket;
    bool         connected;

  public:
    Session();
    ~Session();
    
    bool Connect();
    bool Disconnect();

    bool IsConnected() const
    {
        return connected;
    }

    void SetConnected(bool c)
    {
        connected = c;
    }
};

#endif
