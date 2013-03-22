#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <sys/epoll.h>
#include <pthread.h>

#include <iostream>
#include <sstream>
#include <string>

#include "../../threading/method-request.h"
#include "../../shared/networking/socket-base.h"
#include "../../shared/callback.h"
#include "../network-manager.h"

using namespace std;

class NetworkManager;

#define MAXEVENTS 64
/*
void handle_session_manager_task(void *ptr)
{
    s_manager->AddTaskToServe(ptr);
}*/

class SocketServer: public MethodRequest
{
    private:

        struct epoll_event event, *events;
        struct sockaddr_in serveraddr, clientaddr;
        struct addrinfo serverinfo, *serverinfo_res;

        int sock_listen;
        int epoll_fd;

        void SetupAddrInfo(int family, int socktype, int protocol);
        void SetBlocking(int, const bool) throw(SocketException);

        void SetupEpoll() throw(SocketException);
        void SetupSocket(int port) throw(SocketException);

        EventCallback<void, void*> cb_notify;

        //friend void* EpollThread(void* arg);
        friend class SocketThread;

        NetworkManager& m_netmanager;
        uint32 m_diff;
        bool active;

    public:

        SocketServer(NetworkManager& netmanager, uint32 d) throw(SocketException);
        ~SocketServer();

        void Init(int) throw(SocketException);

        int Call();

        
    //int  Start();

    /*void InitCallback(void (*fptr)(void*))
    {
        //cb_notify->RegisterCb(fptr);
    }*/
};

#endif
