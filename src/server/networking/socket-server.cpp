#include "socket-server.h"

/************** SocketException Code **************/

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

/************** SocketServer Code **************/

SocketServer::SocketServer() throw(SocketException)
{

}

SocketServer::~SocketServer()
{
    //::close(sock_listen);
    //sock_listen = INVALID_SOCKET;
}

void SocketServer::init(int port) throw(SocketException)
{
    setupAddrInfo(AF_INET, SOCK_STREAM , 0);
    setupSocket(port);
    setupEpoll();
}

void SocketServer::setupAddrInfo(int family, int socktype, int protocol)
{
    memset(&serverinfo, 0, sizeof(serverinfo));
    serverinfo.ai_family   = family;
    serverinfo.ai_socktype = socktype;
    serverinfo.ai_protocol = protocol;
}

void SocketServer::setupSocket(int port) throw(SocketException)
{
    int yes = 1;

    if ((sock_listen = ::socket(serverinfo.ai_family, serverinfo.ai_socktype, serverinfo.ai_protocol)) < 0)
        throw SocketException("[socket()]", true);

    if (setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
        throw SocketException("[setsockopt()]", true);

    serveraddr.sin_family = AF_UNSPEC;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(port);
    memset(&(serveraddr.sin_zero), '\0', 8); // ipv6

    if (bind(sock_listen, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        throw SocketException("[bind()]", true);

    if (listen(sock_listen, MAX_LISTEN_QUEUE) < 0)
        throw SocketException("[listen()]", true);

    sock_max = sock_listen;
}

void SocketServer::setupEpoll() throw(SocketException)
{
    epoll_fd = -1;

    events = (struct epoll_event*) calloc(MAX_CONNECTIONS, sizeof(struct epoll_event));

    if ((epoll_fd = epoll_create(MAX_CONNECTIONS)) < 0)
        throw SocketException("[epoll_create()]", true);

    listen_event.events = EPOLLIN;
    listen_event.data.fd = sock_max;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_max, &listen_event) < 0)
        throw SocketException("[epoll_ctl()]", true);
}

void* epoll_thread(void* arg)
{
    SocketServer *srv = (SocketServer*) arg;
    int res = -1, addrlen, sock_client, new_fd, nbytes, i = 0;
    char buf[1024]; // temp

    while(1)
    {
        try
        {
            res = epoll_wait(srv->epoll_fd, srv->events, MAX_CONNECTIONS, -1);


            for (i = 0; i < res; i++)
            {   
                sock_client = srv->events[i].data.fd;

                if(sock_client == srv->sock_listen)
                {
                    addrlen = sizeof(srv->clientaddr);
                    if((new_fd = accept(srv->sock_listen, (struct sockaddr *)&srv->clientaddr, (socklen_t*)&addrlen)) < 0)
                        throw SocketException("[accept()]", true);
                    else
                    {
                        srv->listen_event.events = EPOLLIN | EPOLLET;
                        srv->listen_event.data.fd = new_fd;
                        if (epoll_ctl(srv->epoll_fd, EPOLL_CTL_ADD, new_fd, &srv->listen_event) < 0)
                            throw SocketException("[epoll_ctl()]", true);
                    }
                    break;
                }
                else
                {
                    if (srv->events[i].events & EPOLLHUP == EPOLLHUP)
                    {
                        if (epoll_ctl(srv->epoll_fd, EPOLL_CTL_DEL, sock_client, &srv->listen_event) < 0)
                            throw SocketException("[epoll_ctl()]", true);
                        close(sock_client);
                        break;
                    }

                    if (srv->events[i].events & EPOLLIN == EPOLLIN)
                    {
                        if((nbytes = recv(sock_client, buf, sizeof(buf), 0)) <= 0)  // read header, len, msg
                        {
                            if(nbytes == 0)
                                printf("recv() hung up on socket %d\n", sock_client);
                            else
                                printf("recv() error on socket %d", sock_client);

                            if (epoll_ctl(srv->epoll_fd, EPOLL_CTL_DEL, sock_client, &srv->listen_event) < 0)
                                throw SocketException("[epoll_ctl()]", true);
                            close(sock_client);
                            printf("killing client\n");
                        }
                        else
                        {
                            buf[nbytes] = '\0';
                            printf("[client %d] %s",sock_client, buf);
                            // notify bufmsg
                        }
                        break;
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
