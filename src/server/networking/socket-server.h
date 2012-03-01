#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <netdb.h> // for addrinfo
#include <errno.h>
#include <pthread.h>
#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <exception>
#include <string>

using namespace std;

#define MAX_LISTEN_QUEUE 16
#define MAX_CONNECTIONS (1200)

class SocketException : public exception 
{
  public:
    SocketException(const std::string &message, bool inclSysMsg = false) throw();
    ~SocketException() throw();

    const char *what() const throw();

  private:
    std::string userMessage;
};

void* epoll_thread(void* arg);

class SocketServer
{
    struct epoll_event listen_event, *events;

    struct sockaddr_in serveraddr, clientaddr;
    struct addrinfo serverinfo, *serverinfo_res;

    int sock_listen, sock_max;
    int epoll_fd;

    void setupSocket(int) throw(SocketException);
    void setupEpoll() throw(SocketException);

    friend void* epoll_thread(void* arg);

  public:
    SocketServer() throw(SocketException);
    ~SocketServer();

    void setupAddrInfo(int family, int socktype, int protocol);
    void init(int) throw(SocketException);
};
