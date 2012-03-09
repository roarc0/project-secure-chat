#ifndef _SOCKET_BASE_H
#define _SOCKET_BASE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "../exception.h"

class SocketServerException : public Exception;

class SocketBase
{
  public:
    SocketBase();
    SocketBase(int sock);
    ~SocketBase();

  private:
    int sock;

};

#endif
