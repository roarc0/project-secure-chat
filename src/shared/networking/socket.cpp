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
        throw SocketException("Failed to resolve name (gethostbyname())");
    }
    addr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);

    addr.sin_port = htons(port);     // Assign port in network byte order
}

// Socket Code

Socket::Socket(int type, int protocol, bool block) throw(SocketException) 
{
    int ret, val = 1;

    // Make a new socket
    if ((sockDesc = socket(PF_INET, type, protocol)) < 0)
    {
        throw SocketException("Socket creation failed (socket())", true);
    }

    Socket::block = block;
    setBlocking(block);
    if(!block)
    {
        FD_ZERO(&fd_sock); // per usare la select
    }

    if (setsockopt(sockDesc, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) < 0)
    {
        throw SocketException("Set of socket options failed (setsockopt())", true);
    }

}

Socket::Socket(int sockDesc,  bool block)
{
    this->sockDesc = sockDesc;
    Socket::block = block;
    setBlocking(block);
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

string Socket::getLocalAddress() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of local address failed (getsockname())", true);
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short Socket::getLocalPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getsockname(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of local port failed (getsockname())", true);
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
        throw SocketException("Set of local port failed (bind())", true);
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
        throw SocketException("Set of local address and port failed (bind())", true);
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
    int opts;
    opts = fcntl (sockDesc, F_GETFL);
    if (opts < 0)
    {
        throw SocketException("Get opts failed (setBlocking())", true);
    }
    if (b)
        opts |= O_NONBLOCK;
    else
        opts &= ~O_NONBLOCK ;

    block = b;
    fcntl (sockDesc, F_SETFL, opts);
}

// CommunicatingSocket Code
CommunicatingSocket::CommunicatingSocket(int type, int protocol, bool block)
    throw(SocketException) : Socket(type, protocol, block) 
{
}

CommunicatingSocket::CommunicatingSocket(int newConnSD, bool block) : Socket(newConnSD, block) 
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
        throw SocketException("Connect failed (connect())", true);
    }
}

void CommunicatingSocket::disconnect() throw(SocketException) 
{
    if (close(sockDesc) < 0)
    {
        throw SocketException("Socket close failed (close())", true);
    }
    sockDesc = INVALID_SOCKET;
}

void CommunicatingSocket::send(const void *buffer, int bufferLen) 
    throw(SocketException) 
{
    if (::send(sockDesc, (raw_type *) buffer, bufferLen, 0) < 0) 
    {
        throw SocketException("Send failed (send())", true);
    }
}

int CommunicatingSocket::recv(void *buffer, int bufferLen) 
    throw(SocketException) 
{
    int ret;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100;

    if (!block)
    {
        FD_ZERO(&fd_sock);
        FD_SET(sockDesc, &fd_sock);

        //if((ret = select(sockDesc+1, &fd_sock, NULL, NULL, &tv)) < 0)
        //    throw SocketException("Receive failed (select())", true);

        if (FD_ISSET(sockDesc, &fd_sock) == 0)
            throw SocketException("Receive failed (FD_ISSET())", true);         // connessione fallita client disconnesso
    }

    if ((ret = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0)) < 0)
        throw SocketException("Receive failed (recv())", true);

    if (!block)
    {
        FD_CLR(sockDesc, &fd_sock);
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
        throw SocketException("Fetch of foreign address failed (getpeername())", true);
    }
    return inet_ntoa(addr.sin_addr);
}

unsigned short CommunicatingSocket::getForeignPort() throw(SocketException) 
{
    sockaddr_in addr;
    unsigned int addr_len = sizeof(addr);

    if (getpeername(sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
    {
        throw SocketException("Fetch of foreign port failed (getpeername())", true);
    }
    return ntohs(addr.sin_port);
}

// TCPSocket Code
TCPSocket::TCPSocket() 
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP) 
{
}

TCPSocket::TCPSocket(const string &foreignAddress, unsigned short foreignPort,  bool block)
    throw(SocketException) : CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP,block) 
{
    connect(foreignAddress, foreignPort);
}

TCPSocket::TCPSocket(int newConnSD, bool block) : CommunicatingSocket(newConnSD, block) 
{
}

// TCPServerSocket Code

TCPServerSocket::TCPServerSocket(unsigned short localPort, bool block, int queueLen)
    throw(SocketException) : Socket(SOCK_STREAM, IPPROTO_TCP, block) 
{
    setLocalPort(localPort);
    setListen(queueLen);
    setBlocking(block);
}

TCPServerSocket::TCPServerSocket(const string &localAddress, 
    unsigned short localPort, bool block, int queueLen) 
    throw(SocketException) : Socket(SOCK_STREAM, IPPROTO_TCP, block) 
{
    setLocalAddressAndPort(localAddress, localPort);
    setListen(queueLen);
    setBlocking(block);
}

TCPSocket *TCPServerSocket::accept() throw(SocketException) 
{
    int newConnSD;
    if ((newConnSD = ::accept(sockDesc, NULL, 0)) < 0) 
    {
        throw SocketException("Accept failed (accept())", true);
    }

    return new TCPSocket(newConnSD, block);
}

void TCPServerSocket::setListen(int queueLen) throw(SocketException) 
{
    if (!block)
        FD_SET(sockDesc, &fd_sock);

    if (listen(sockDesc, queueLen) < 0) 
    {
        throw SocketException("Set listening socket failed (listen())", true);
    }
}
