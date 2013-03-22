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

#define MAXEVENTS 64

/*
void handle_new_connection(void *ptr)
{
    s_manager->AddSession(ptr);
}*/

class SocketServer: public MethodRequest
{
    private:

        struct epoll_event event, *events;
        struct sockaddr_in serveraddr, clientaddr;
        struct addrinfo serverinfo, *serverinfo_res;

        int sock_listen;
        int epoll_fd;
        
        //TODO da estirpare
        Session_smart s;

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

        int Call();

    /*void InitCallback(void (*fptr)(void*))
    {
        //cb_notify->RegisterCb(fptr);
    }*/
};

#endif
