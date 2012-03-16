#include "socket-client.h"

SocketClient::SocketClient(int type, int protocol) throw(SocketException) 
{
    domain = AF_INET;
    SocketClient::type = type;
    SocketClient:: protocol = protocol;
    initSocket();
}

SocketClient::~SocketClient()
{
    close(sockDesc);
    sockDesc = INVALID_SOCKET;
}

void SocketClient::Connect(const string &foreignAddress,
    unsigned short foreignPort) throw(SocketException) 
{
    sockaddr_in destAddr;
    fillAddr(foreignAddress, foreignPort, destAddr);

    if (::connect(sockDesc, (sockaddr *) &destAddr, sizeof(destAddr)) < 0)
    {
        //close(sockDesc); // per la riconnessione
        throw SocketException("Connect failed [connect()]", true);
    }
}

void SocketClient::Disconnect() throw(SocketException) 
{
    if (close(sockDesc) < 0)
    {
        throw SocketException("Socket close failed [close()]", true);
    }
    sockDesc = INVALID_SOCKET;
    initSocket();
}
