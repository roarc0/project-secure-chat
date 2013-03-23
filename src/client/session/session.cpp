#include "session.h"

Session::Session() //: SessionBase()
{
    connected = false;
    c_Socket = new SocketClient(SOCK_STREAM, 0);
    m_Socket = (SocketBase*) c_Socket;
}

Session::~Session()
{
    // TODO
}

bool Session::Connect()
{
    try
    {
        c_Socket->Connect(CFG_GET_STRING("server_host"),
                          CFG_GET_INT("server_port"));
    }
    catch(SocketException &e)
    {
        INFO("debug", "connection failed     %s:%d (%s)\n",
             CFG_GET_STRING("server_host").c_str(),
             CFG_GET_INT("server_port"), e.what());
        SetConnected(false);

        return false;
    }

    INFO("debug", "connection successful %s:%d\n",
         CFG_GET_STRING("server_host").c_str(),
         CFG_GET_INT("server_port"));
    SetConnected(true);

    return true;
}

bool Session::Disconnect()
{
    assert(session);

    try
    {
        c_Socket->Disconnect();
        SetConnected(false);
    }
    catch(SocketException &e)
    {
        INFO("debug", "disconnection failed     %s:%d (%s)\n",
             CFG_GET_STRING("server_host").c_str(),
             CFG_GET_INT("server_port"), e.what());

        return false;
    }

    INFO("debug", "disconnection successful %s:%d\n",
         CFG_GET_STRING("server_host").c_str(),
         CFG_GET_INT("server_port"));

    return true;
}
