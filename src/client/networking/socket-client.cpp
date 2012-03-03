#include "socket-client.h"

// Function to fill in address structure given an address and port
static void fillAddr(const string &address, unsigned short port, 
                     sockaddr_in &addr) 
{
    memset(&addr, 0, sizeof(addr));  // Zero out address structure
    addr.sin_family = AF_INET;       // Internet address

    hostent *host;  // Resolve name
    if ((host = gethostbyname(address.c_str())) == NULL) 
    {
        throw SocketException("Failed to resolve name [gethostbyname()]");
    }
    addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
    addr.sin_port = htons(port);     // Assign port in network byte order
}

// Socket Code

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

void SocketClient::initSocket() throw(SocketException)
{
    int ret, val = 1;

    if ((sockDesc = socket(AF_INET, type, protocol)) < 0)
        throw SocketException("Socket creation failed [socket()]", true);
}

string SocketClient::getLocalAddress() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of local address failed [getsockname()]", true);
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short SocketClient::getLocalPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of local port failed [getsockname()]", true);
    }
    return ntohs(addr.sin_port);
}

void SocketClient::setLocalPort(unsigned short localPort) throw(SocketException) 
{
    // Bind the socket to its port
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

void SocketClient::setLocalAddressAndPort(const string &localAddress,
    unsigned short localPort) throw(SocketException) 
{
    // Get the address of the requested host
    sockaddr_in localAddr;
    fillAddr(localAddress, localPort, localAddr);

    if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) 
    {
        throw SocketException("Set of local address and port failed [bind()]", true);
    }
}

unsigned short SocketClient::resolveService(const string &service,
                                      const string &protocol) 
{
    struct servent *serv;

    if ((serv = getservbyname(service.c_str(), protocol.c_str())) == NULL)
        return atoi(service.c_str());
    else 
        return ntohs(serv->s_port);
}

void SocketClient::connect(const string &foreignAddress,
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

void SocketClient::disconnect() throw(SocketException) 
{
    if (close(sockDesc) < 0)
    {
        throw SocketException("Socket close failed [close()]", true);
    }
    sockDesc = INVALID_SOCKET;
    initSocket();
}

void SocketClient::send(const void *buffer, int bufferLen) 
    throw(SocketException) 
{
    if (::send(sockDesc, (void *) buffer, bufferLen, 0) < 0) 
    {
        throw SocketException("Send failed [send()]", true);
    }
}

int SocketClient::recv(void *buffer, int bufferLen) 
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

string SocketClient::getForeignAddress() 
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

unsigned short SocketClient::getForeignPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0)
    {
        throw SocketException("Fetch of foreign port failed [getpeername()]", true);
    }
    return ntohs(addr.sin_port);
}
