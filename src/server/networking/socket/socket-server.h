#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <sys/epoll.h>
#include <pthread.h>

#include <iostream>
#include <sstream>
#include <string>

#include "threading/method-request.h"
#include "networking/socket-base.h"
#include "utility/callback.h"
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

    pthread_mutex_t mutex_events;

    void SetupAddrInfo(int family, int socktype, int protocol);
    void SetBlocking(int, const bool) throw(SocketException);

    void SetupEpoll() throw(SocketException);
    void SetupSocket(int port) throw(SocketException);

    EventCallback<void, void*> cb_notify;


    NetworkManager& m_netmanager;
    uint32 m_diff;
    bool active;

  public:
    SocketServer(NetworkManager& netmanager, uint32 d) throw(SocketException);
    ~SocketServer();

    void Init(int) throw(SocketException);
    int  Call();
    void Kill(int) throw(SocketException);
};

#endif
