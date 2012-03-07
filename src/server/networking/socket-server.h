#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <netdb.h> // for addrinfo
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <sstream>
#include <exception>
#include <string>

#include "../../shared/utility/exception.h"
#include "../../shared/callback/callback.h"
#include "../session/queue.h"

using namespace std;

#define MAX_CONNECTIONS (64)

void* epoll_thread(void* arg);

net_task new_connection_net_task(int sock)
{
    net_task n_task;
    n_task.ptr = (void*) sock;
    n_task.p_pack = NULL;
    n_task.type_task = NEW;
    return n_task;
}

class SocketServerException : public Exception;

class SocketServer
{
    struct epoll_event event, *events;

    struct sockaddr_in serveraddr, clientaddr;
    struct addrinfo serverinfo, *serverinfo_res;

    int sock_listen;
    int epoll_fd;

    /*throw(SocketException);
    inline void setupAddrInfo(int family, int socktype, int protocol);
    inline void setBlocking(int, const bool) throw(SocketException);*/

    event_callback<void, net_task> cb_notify;

    friend void* epoll_thread(void* arg);

  public:
    SocketServer() throw(SocketException);
    ~SocketServer();

    void init(int) throw(SocketException);

    void init_callback(void (*fptr)(net_task))
    {
        cb_notify->register_cb(fptr);
    }

    /*void new_connection_init()
    {
        b_new_connection = false;
        pthread_cond_init(&c_new_connection);
        pthread_mutex_init(&m_new_connection);
        pthread_mutex_lock(&m_new_connection);
    }

    void new_connection_cond_signal()
    {

        pthread_cond_signal(&c_new_connection);
    }
    void new_connection_cond_wait()
    {
        pthread_cond_wait(&c_new_connection);
    }

    void new_connection_enter_lock()
    {
        pthread_mutex_lock(&m_new_connection_enter);
    }

    void new_connection_exit_unlock()
    {
        pthread_mutex_lock(&m_new_connection_exit);
    }*/
};

#endif
