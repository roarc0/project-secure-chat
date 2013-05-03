#ifndef _SESSION_H
#define _SESSION_H

#include "session/sessionbase.h"
#include "../networking/socket-client.h"
#include "utility/counted_ptr.h"
#include "xml-message.h"

class Session;
typedef counted_ptr<Session> Session_smart;

class Session : public SessionBase
{
    SocketClient* c_Socket;
    string        m_pwd_digest;
  public:
    Session();
    ~Session();

    bool Connect();
    bool Disconnect();
    void ResetSocket();

    bool Update();    

    void HandleClientSide(Packet& /*packet*/);    
    void HandlePing(Packet& /*packet*/);
    void HandleMessage(Packet& /*packet*/);
    void HandleServerMessage(Packet& /*packet*/);
    void HandleJoinChannel(Packet& /*packet*/);
    void HandleLeaveChannel(Packet& /*packet*/);
    void HandleLogin(Packet& packet);
    bool HandleSend(const char* msg);

    void SetPassword(const char *);
    bool IsPasswordSet();
    void ClearPassword();
    
    void SendToGui(std::string str, std::string nick, char type);

  private:

    bool Update(uint32 diff);
    
};

#endif
