#include "socket-exception.h"

SocketException::SocketException(const string &message, bool perr)
  throw() : userMessage(message)
{
    if (perr)
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
