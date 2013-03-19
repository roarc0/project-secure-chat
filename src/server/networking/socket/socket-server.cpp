#include "socket-server.h"

SocketServer::SocketServer() throw(SocketException)
{
    //new_connection_init();
}

SocketServer::~SocketServer()
{
    ::close(sock_listen);
    sock_listen = INVALID_SOCKET;
}

void SocketServer::Init(int port) throw(SocketException)
{
    SetupAddrInfo(AF_UNSPEC, SOCK_STREAM , 0);
    SetupSocket(port);
    SetupEpoll();
}

inline void SocketServer::SetBlocking(int sock, const bool block)
    throw(SocketException)
{
    int flags;

    if ((flags = fcntl (sock, F_GETFL, 0)) < 0)
        throw SocketException("[setBlocking() -> fnctl()]", true);

    if (!block)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK ;

    if(fcntl (sock, F_SETFL, flags) < 0)
        throw SocketException("[setBlocking() -> fnctl()]", true);
}

inline void SocketServer::SetupAddrInfo(int family, int socktype, int protocol)
{
    memset(&serverinfo, 0, sizeof(struct addrinfo));
    serverinfo.ai_family   = family;
    serverinfo.ai_socktype = socktype;
    //serverinfo.ai_protocol = protocol;
    serverinfo.ai_flags    = AI_PASSIVE;
}

void SocketServer::SetupSocket(int port) throw(SocketException)
{
    stringstream s_port;
    int yes = 1;
    struct addrinfo *ai_res;

    s_port << port;

    if (getaddrinfo (NULL, s_port.str().c_str(), &serverinfo, &serverinfo_res) < 0)
        throw SocketException("[getaddrinfo()]", true);

    for (ai_res = serverinfo_res; ai_res != NULL; ai_res = ai_res->ai_next)
    {
        if ((sock_listen = socket(ai_res->ai_family, ai_res->ai_socktype, ai_res->ai_protocol)) < 0)
            continue;

        //if (setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
        //    throw SocketException("[setsockopt()]", true);

        if (bind(sock_listen, ai_res->ai_addr, ai_res->ai_addrlen) == 0)
            break;

        close(sock_listen);
    }

    if (serverinfo_res == NULL)
        throw SocketException("bind failed!", false);
    
    freeaddrinfo (serverinfo_res);

    SetBlocking(sock_listen, false);

    if (listen(sock_listen, SOMAXCONN) < 0)
        throw SocketException("[listen()]", true);
}

void SocketServer::SetupEpoll() throw(SocketException)
{
    events = (struct epoll_event*) calloc(MAX_CONNECTIONS, sizeof(struct epoll_event));

    if ((epoll_fd = epoll_create1(0)) < 0)
        throw SocketException("[epoll_create()]", true);

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = sock_listen;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_listen, &event) < 0)
        throw SocketException("[epoll_ctl()]", true);
}

void* EpollThread(void* arg)
{
    SocketServer *srv = (SocketServer*) arg;
    int res = -1, sock_new, nbytes, i = 0;

    while(1)
    {
        try
        {
            if ((res = epoll_wait(srv->epoll_fd, srv->events, MAX_CONNECTIONS, -1)) < 0)
                throw SocketException("[epoll_wait()]", true);

            for (i = 0; i < res; i++)
            {
                if ((srv->events[i].events & EPOLLERR) ||
                    (srv->events[i].events & EPOLLHUP) ||
                    (!(srv->events[i].events & EPOLLIN)))
                {
                    fprintf(stderr, "epoll error\n");
                    close(srv->events[i].data.fd);
                    continue;
                }

                if (srv->sock_listen == srv->events[i].data.fd)
                {
                    while (1)
                    {
                        struct sockaddr in_addr;
                        socklen_t       in_len;
                        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                        in_len = sizeof in_addr;

                        if ((sock_new = accept (srv->sock_listen, &in_addr, &in_len)) < 0)
                        {
                            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                                break;
                            else
                            {
                                perror("accept");
                                break;
                            }
                        }

                        if (getnameinfo(&in_addr, in_len,
                                        hbuf, sizeof hbuf,
                                        sbuf, sizeof sbuf,
                                        NI_NUMERICHOST | NI_NUMERICSERV) == 0)
                        {
                            printf("client %d "
                                   "(host=%s, port=%s)\n", sock_new, hbuf, sbuf);
                        }
                        else
                            throw SocketException("[getnameinfo()]", true);

                        srv->SetBlocking(sock_new, false);

                        srv->event.data.fd = sock_new;
                        srv->event.events = EPOLLIN | EPOLLET;

                        if (epoll_ctl (srv->epoll_fd, EPOLL_CTL_ADD, sock_new, &srv->event) < 0)
                            throw SocketException("[epoll_ctl()]", true);

                        //cb_notify(new_connection_net_task());
                    }

                    continue;
	            }
                else
                {
                    bool end = 0;
                    while (1)
                    {
                      ssize_t nbytes;
                      char buf[512];

                      if ((nbytes = read (srv->events[i].data.fd, buf, sizeof buf)) < 0)
                      {
                        if (errno != EAGAIN)  /* If errno == EAGAIN, that means we have read all data.*/
                        {
                          perror ("read");
                          end = true;
                        }
                        break;
                      }
                      if (nbytes == 0)
                      {
                          end = true;
                          break;
                      }

                      buf[nbytes] = '\0';
                      printf("client %d : %s", 
                             srv->events[i].data.fd, buf);
                    }

                    if (end)
                    {
                        printf("client %d connection closed\n",
                               srv->events[i].data.fd);
                        close(srv->events[i].data.fd);
                    }
                }
            }
        }
        catch(SocketException e)
        {
            cout << e.what() << endl;
        }
    }

    pthread_exit(NULL);
}
