#ifndef _SESSION_H
#define _SESSION_H

#include "session/sessionbase.h"
#include "../networking/socket-client.h"
#include "utility/counted_ptr.h"

class Session;
typedef counted_ptr<Session> Session_smart;

class Session : public SessionBase
{
    SocketClient* c_Socket;
    bool         connected;

  public:
    Session();
    ~Session();

    bool Connect();
    bool Disconnect();
    void ResetSocket();
    bool IsConnected() const
    {
        return connected;
    }

    void SetConnected(bool c)
    {
        connected = c;
    }

    bool Update(uint32 diff);

    void Handle_ClientSide(Packet& /*packet*/);
    void HandleMessage(Packet& /*packet*/);
    void Handle_Ping(Packet& /*packet*/);
};

#endif
