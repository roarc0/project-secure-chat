#include "socket-base.h"

bool FillAddr(const string &address, unsigned short port, 
                     sockaddr_in &addr) 
{
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    hostent *host;
    if ((host = gethostbyname(address.c_str())) == NULL) 
    {
        throw SocketException("Failed to resolve name (gethostbyname())", true);
    }

    //INFO("debug","* ip: %s\n", inet_ntoa(*((struct in_addr *)host->h_addr_list[0])));
    addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
    addr.sin_port = htons(port);
    return true;
}

SocketBase::SocketBase(int type, int protocol) throw(SocketException)
{
    sock = INVALID_SOCKET;
    domain = AF_INET;
    SocketBase::type = type;
    SocketBase::protocol = protocol;
    InitSocket();
}

SocketBase::SocketBase(int socket)
{
    sock = socket;
    domain = AF_INET;
}

SocketBase::~SocketBase()
{
    close(sock);
    sock = INVALID_SOCKET;
}

void SocketBase::InitSocket() throw(SocketException)
{
    if ((sock = socket(AF_INET, type, protocol)) < 0)
        throw SocketException("[socket()]", true);
}

void SocketBase::CloseSocket()
{
    close(sock);
    sock = INVALID_SOCKET;
}

bool SocketBase::IsClosed()
{
    return sock == INVALID_SOCKET;
}

int SocketBase::GetSocket() const
{
    return sock;
}

string SocketBase::GetLocalAddress() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sock, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("[getsockname()]", true);
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short SocketBase::GetLocalPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sock, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("[getsockname()]", true);
    }
    return ntohs(addr.sin_port);
}

void SocketBase::SetLocalPort(unsigned short localPort) throw(SocketException) 
{
    sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(localPort);

    if (bind(sock, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) 
    {
        throw SocketException("Set of local port failed [bind()]", true);
    }
}

void SocketBase::SetLocalAddressAndPort(const string &localAddress,
    unsigned short localPort) throw(SocketException) 
{
    sockaddr_in localAddr;
    FillAddr(localAddress, localPort, localAddr);

    if (bind(sock, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) 
    {
        throw SocketException("Set of local address and port failed [bind()]", true);
    }
}

unsigned short SocketBase::ResolveService(const string &service,
                                      const string &protocol) 
{

    struct servent *serv;
    if ((serv = getservbyname(service.c_str(), protocol.c_str())) == NULL)
        return atoi(service.c_str());
    else 
        return ntohs(serv->s_port);
}

void SocketBase::Send(const void *buffer, int bufferLen) 
    throw(SocketException) 
{
    if (::send(sock, (void *) buffer, bufferLen, 0) < 0) 
    {
        throw SocketException("Send failed [send()]", true);
    }
}

int SocketBase::Recv(void *buffer, int bufferLen) 
    throw(SocketException) 
{
    int ret;

    if ((ret = ::recv(sock, (void *) buffer, bufferLen, 0)) <= 0)
    {
        close(sock);
        sock = INVALID_SOCKET;
        //InitSocket();

        if (ret == 0)
            throw SocketException("Receive failed [recv()]", false);
        else
            throw SocketException("Receive failed [recv()]", true);
    }

    return ret;
}

string SocketBase::GetForeignAddress() 
    throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sock, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign address failed [getpeername()]", true);
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short SocketBase::GetForeignPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sock, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign port failed [getpeername()]", true);
    }
    return ntohs(addr.sin_port);
}
