#include "socket-client.h"

void SocketClient::Connect(const string &foreignAddress,
    unsigned short foreignPort) throw(SocketException)
{
    sockaddr_in destAddr;
    FillAddr(foreignAddress, foreignPort, destAddr);

    if (::connect(sock, (sockaddr *) &destAddr, sizeof(destAddr)) < 0)
    {
        //close(sock); // per la riconnessione
        throw SocketException("Connect failed [connect()]", true);
    }
}

void SocketClient::Disconnect() throw(SocketException)
{
    if (close(sock) < 0)
    {
        throw SocketException("Socket close failed [close()]", true);
    }
}
