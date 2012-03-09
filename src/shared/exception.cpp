#include "socket-exception.h"

Exception::Exception(const string &message, bool perr)
  throw() : uMessage(message)
{
    if (perr)
    {
        uMessage.append(": ");
        uMessage.append(strerror(errno));
    }
}

Exception::~Exception() throw()
{
}

const char *Exception::what() const throw()
{
    return uMessage.c_str();
}
