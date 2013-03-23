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
/*
void SessionBase::SendPacket(Packet* new_packet)
{
    if (!m_Socket || !new_packet)
        return;

    if (_SendPacket(*new_packet) == -1)
        m_Socket->CloseSocket();
}

int SessionBase::_SendPacket(const Packet& pct)
{
    PktHeader header(pct.size()/+OPCODE_SIZE/, pct.GetOpcode());

    unsigned char* rawData = 
        new unsigned char[header.getHeaderLength()+ pct.size() + 1];
    
    // Inserire Criptazione

    memcpy((void*)rawData,
           (char*) header.header,
           header.getHeaderLength());
    memcpy((void*)rawData + header.getHeaderLength(),
           (char*) pct.contents(),
           pct.size());

    m_Socket->Send(rawData, pct.size() + header.getHeaderLength());
    delete[] rawData;
}

void Session::ReceivePacket(Packet *pack)
{
    uint16 len = 0,bread, opcode;
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
        *pack << opcode;
        //if(opcode >= MAX_OPCODE) boommm!!!

        if (m_Socket->Recv(&len, LENGTH_SIZE) <= 0)
        {
            perror("recv");
            return;
        }
        *pack << len;
        //limitare la lunghezza. massima

        buffer = new char[len+1];
        if((bread = m_Socket->Recv(buffer, len)) <= 0)
        {
            perror("recv");
            return;
        }
        buffer[bread] = '\0';
        INFO("debug","receiving message: opcode %d, length %d, data \"%s\"\n",
                     opcode, len, buffer);
        *pack << buffer;
        
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
*/
