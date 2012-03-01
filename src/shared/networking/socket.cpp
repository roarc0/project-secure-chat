#include "socket.h"
#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
typedef void raw_type;

// SocketException Code
SocketException::SocketException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) 
{
    if (inclSysMsg) 
    {
        userMessage.append(": ");
        userMessage.append(strerror(errno));
    }
}

SocketException::~SocketException() throw() 
{
}

const char *SocketException::what() const throw() 
{
    return userMessage.c_str();
}

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

Socket::Socket(int type, int protocol, bool s_block) throw(SocketException) 
{
    domain = AF_INET;
    Socket::type = type;
    Socket:: protocol = protocol;
    block = s_block;
    initSocket();
}

Socket::Socket(int sockDesc, int type, int protocol, bool s_block)
{
    domain = AF_INET;
    Socket::type = type;
    Socket:: protocol = protocol;
    this->sockDesc = sockDesc;
    block = s_block;
    //setBlocking(block);
}

Socket::~Socket()
{
    if (!block)
    {
        if (FD_ISSET(sockDesc, &fd_sock))
            FD_CLR(sockDesc, &fd_sock);
        FD_ZERO(&fd_sock);
    }

    ::close(sockDesc);
    sockDesc = INVALID_SOCKET;
}

void Socket::initSocket() throw(SocketException)
{
    int ret, val = 1;

    // Make a new socket
    if ((sockDesc = socket(AF_INET, type, protocol)) < 0)
        throw SocketException("Socket creation failed [socket()]", true);

    if (setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) < 0)
        throw SocketException("Set of socket options failed [setsockopt()]", true);

    setBlocking(block);
    if(!block)
        FD_ZERO(&fd_sock); // per usare la select    

    INFO("debug", "* init socket=%d block=%d\n", sockDesc, block);
}

string Socket::getLocalAddress() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of local address failed [getsockname()]", true);
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short Socket::getLocalPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of local port failed [getsockname()]", true);
    }
    return ntohs(addr.sin_port);
}

void Socket::setLocalPort(unsigned short localPort) throw(SocketException) 
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

void Socket::setLocalAddressAndPort(const string &localAddress,
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

unsigned short Socket::resolveService(const string &service,
                                      const string &protocol) 
{
    struct servent *serv;        /* Structure containing service information */

    if ((serv = getservbyname(service.c_str(), protocol.c_str())) == NULL)
        return atoi(service.c_str());  /* Service is port number */
    else 
        return ntohs(serv->s_port);    /* Found port (network byte order) by name */
}

void Socket::setBlocking(const bool b)
    throw(SocketException)
{
    INFO("debug", "* setting blocking socket=%d block=%d\n",sockDesc, block);

    int opts;
    opts = fcntl (sockDesc, F_GETFL);
    if (opts < 0)
        throw SocketException("Get opts failed [setBlocking()]", true);

    if (b)
        opts |= O_NONBLOCK;
    else
        opts &= ~O_NONBLOCK ;

    block = b;
    fcntl (sockDesc, F_SETFL, opts);
}

// CommunicatingSocket Code
CommunicatingSocket::CommunicatingSocket(int type, int protocol, bool s_block)
    throw(SocketException) : Socket(type, protocol, s_block) 
{
}

CommunicatingSocket::CommunicatingSocket(int newConnSD, int type, int protocol, bool s_block) : Socket(newConnSD, type, protocol, s_block) 
{
}

void CommunicatingSocket::connect(const string &foreignAddress,
    unsigned short foreignPort) throw(SocketException) 
{
    // Get the address of the requested host
    sockaddr_in destAddr;
    fillAddr(foreignAddress, foreignPort, destAddr);

    // Try to connect to the given port
    if (::connect(sockDesc, (sockaddr *) &destAddr, sizeof(destAddr)) < 0)
    {
        //close(sockDesc); // per la riconnessione
        throw SocketException("Connect failed [connect()]", true);
    }
}

void CommunicatingSocket::disconnect() throw(SocketException) 
{
    if (close(sockDesc) < 0)
    {
        throw SocketException("Socket close failed [close()]", true);
    }
    sockDesc = INVALID_SOCKET;
    initSocket();
}

void CommunicatingSocket::send(const void *buffer, int bufferLen) 
    throw(SocketException) 
{
    if (::send(sockDesc, (raw_type *) buffer, bufferLen, 0) < 0) 
    {
        throw SocketException("Send failed [send()]", true);
    }

    INFO("debug", "* %d bytes sent\n", bufferLen);

}

int CommunicatingSocket::recv(void *buffer, int bufferLen) 
    throw(SocketException) 
{
    int ret;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 30;

    if (!block)
    {
        FD_ZERO(&fd_sock);
        FD_SET(sockDesc, &fd_sock);

        ret = select(sockDesc + 1, &fd_sock, NULL, NULL, &tv);

        if (ret == 0)
            return 0;

        if (ret < 0)
            throw SocketException("Receive failed [Select()]", true);         // connessione fallita client disconnesso

        ret = FD_ISSET(sockDesc, &fd_sock);

        if (ret == 0)
            return 0;

        if (ret < 0)
            throw SocketException("Receive failed [FD_ISSET()]", true);         // connessione fallita client disconnesso
    }

    if ((ret = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0)) <= 0)
    {
        close(sockDesc);
        sockDesc = INVALID_SOCKET;
        //initSocket();

        if (ret == 0)
            throw SocketException("Receive failed [recv()]", false);
        else
            throw SocketException("Receive failed [recv()]", true);
    }

    if (!block)
    {
        //FD_CLR(sockDesc, &fd_sock);
        FD_ZERO(&fd_sock);
    }

    return ret;
}

string CommunicatingSocket::getForeignAddress() 
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

unsigned short CommunicatingSocket::getForeignPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of foreign port failed [getpeername()]", true);
    }
    return ntohs(addr.sin_port);
}

// TCPSocket Code
TCPSocket::TCPSocket() 
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP, block) 
{
}

TCPSocket::TCPSocket(const string &foreignAddress, unsigned short foreignPort, bool s_block)
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP, s_block)
{
    connect(foreignAddress, foreignPort);
}

TCPSocket::TCPSocket(int newConnSD, bool s_block) : CommunicatingSocket(newConnSD, SOCK_STREAM, IPPROTO_TCP, s_block) 
{
}

// TCPServerSocket Code

TCPServerSocket::TCPServerSocket(unsigned short localPort, bool s_block, int queueLen)
    throw(SocketException) : Socket(SOCK_STREAM, IPPROTO_TCP, s_block) 
{
    setLocalPort(localPort);
    setListen(queueLen);
}

TCPServerSocket::TCPServerSocket(const string &localAddress, 
    unsigned short localPort, bool s_block, int queueLen) 
    throw(SocketException) : Socket(SOCK_STREAM, IPPROTO_TCP, s_block) 
{
    setLocalAddressAndPort(localAddress, localPort);
    setListen(queueLen);
}

TCPSocket *TCPServerSocket::accept() throw(SocketException) 
{
    int newSockDesc;
    if ((newSockDesc = ::accept(sockDesc, NULL, 0)) < 0) 
    {
        throw SocketException("Accept failed [accept()]", true);
    }

    INFO("debug", "* accepting new socket=%d block=%d\n",newSockDesc, block);

    return new TCPSocket(newSockDesc, block);
}

void TCPServerSocket::setListen(int queueLen) throw(SocketException) 
{
    if (!block)
        FD_SET(sockDesc, &fd_sock);

    if (listen(sockDesc, queueLen) < 0)
    {
        throw SocketException("Set listening socket failed [listen()]", true);
    }
}
