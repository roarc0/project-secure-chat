#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <sys/epoll.h>
#include <pthread.h>

#include <iostream>
#include <sstream>
#include <string>

#include "threading/method-request.h"
#include "networking/socket-base.h"
#include "threading/lock.h"
#include "network-manager.h"
#include "session.h"

using namespace std;

class NetworkManager;

#define MAXEVENTS 256

class SocketServer: public MethodRequest
{
  private:
    struct epoll_event event, *events;
    struct sockaddr_in serveraddr, clientaddr;
    struct addrinfo serverinfo, *serverinfo_res;

    int sock_listen;
    int epoll_fd;

    Mutex mutex_events;

    NetworkManager& m_netmanager;

    void SetupAddrInfo(int family, int socktype, int protocol);
    void SetBlocking(int, const bool) throw(SocketException);

    void SetupEpoll() throw(SocketException);
    void SetupSocket(int port) throw(SocketException);

  public:
    SocketServer(NetworkManager& netmanager, uint32 d) throw(SocketException);
    ~SocketServer();

    void Init(int) throw(SocketException);
    int  Call();
};

#endif
