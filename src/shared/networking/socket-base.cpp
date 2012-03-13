#include "socket-base.h"

static void fillAddr(const string &address, unsigned short port, 
                     sockaddr_in &addr) 
{
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    hostent *host;
    if ((host = gethostbyname(address.c_str())) == NULL) 
    {
        throw SocketException("Failed to resolve name [gethostbyname()]");
    }
    addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
    addr.sin_port = htons(port);
}

SocketBase::SocketBase(int type, int protocol) throw(SocketException) 
{
    domain = AF_INET;
    SocketBase::type = type;
    SocketBase:: protocol = protocol;
    initSocket();
}

SocketBase::~SocketBase()
{
    close(sockDesc);
    sockDesc = INVALID_SOCKET;
}

void SocketBase::initSocket() throw(SocketException)
{
    int ret, val = 1;

    if ((sockDesc = socket(AF_INET, type, protocol)) < 0)
        throw SocketException("Socket creation failed [socket()]", true);
}

string SocketBase::getLocalAddress() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of local address failed [getsockname()]", true);
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short SocketBase::getLocalPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of local port failed [getsockname()]", true);
    }
    return ntohs(addr.sin_port);
}

void SocketBase::setLocalPort(unsigned short localPort) throw(SocketException) 
{
    sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(localPort);

    if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) 
    {
        throw SocketException("Set of local port failed [bind()]", true);
    }
}

void SocketBase::setLocalAddressAndPort(const string &localAddress,
    unsigned short localPort) throw(SocketException) 
{
    sockaddr_in localAddr;
    fillAddr(localAddress, localPort, localAddr);

    if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) 
    {
        throw SocketException("Set of local address and port failed [bind()]", true);
    }
}

unsigned short SocketBase::resolveService(const string &service,
                                      const string &protocol) 
{
    struct servent *serv;

    if ((serv = getservbyname(service.c_str(), protocol.c_str())) == NULL)
        return atoi(service.c_str());
    else 
        return ntohs(serv->s_port);
}

void SocketBase::send(const void *buffer, int bufferLen) 
    throw(SocketException) 
{
    if (::send(sockDesc, (void *) buffer, bufferLen, 0) < 0) 
    {
        throw SocketException("Send failed [send()]", true);
    }
}

int SocketBase::recv(void *buffer, int bufferLen) 
    throw(SocketException) 
{
    int ret;

    if ((ret = ::recv(sockDesc, (void *) buffer, bufferLen, 0)) <= 0)
    {
        close(sockDesc);
        sockDesc = INVALID_SOCKET;
        //initSocket();

        if (ret == 0)
            throw SocketException("Receive failed [recv()]", false);
        else
            throw SocketException("Receive failed [recv()]", true);
    }

    return ret;
}

string SocketBase::getForeignAddress() 
    throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign address failed [getpeername()]", true);
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short SocketBase::getForeignPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign port failed [getpeername()]", true);
    }
    return ntohs(addr.sin_port);
}
