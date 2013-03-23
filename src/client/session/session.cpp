#include "session.h"

Session::Session() //: SessionBase()
{
    connected = false;
    c_Socket = new SocketClient(SOCK_STREAM, 0);
    m_Socket = (SocketBase*) c_Socket;
}

Session::~Session()
{

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

void Session::ReceivePacket(Packet *pack)
{
    uint16 len = 0, opcode;
    char *buffer;
    assert(pack);

    INFO("debug","waiting for data...\n");

    //TODO sparare la roba in packet

    try
    {
        if (m_Socket->Recv(&opcode, OPCODE_SIZE) <= 0)
        {
            perror("recv");
            return;
        }
        INFO("debug","opcode : %d\n", opcode);

        if (m_Socket->Recv(&len, LENGTH_SIZE) <= 0)
        {
            perror("recv");
            return;
        }
        INFO("debug","len    : %d\n", len);

        buffer = new char[len+1];
        if(m_Socket->Recv(buffer, len) <= 0)
        {
            perror("recv");
            return;
        }
        buffer[len] = '\0';
        INFO("debug","msg    : \"%s\"\n", buffer);

        delete buffer;
        buffer = NULL;
    } 
    catch (SocketException e)
    {
        INFO("debug","* client session error, %s\n", e.what());
        SetConnected(false);
        m_Socket->InitSocket();
    }
}
