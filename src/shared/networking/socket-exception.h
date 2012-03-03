#ifndef SOCKET_EXCEPTION_H
#define SOCKET_EXCEPTION_H

#include <exception>
#include <string>
#include <string.h>
#include <errno.h>

using namespace std;

class SocketException : public exception 
{
  public:
    SocketException(const std::string &message, bool perr = false) throw();
    ~SocketException() throw();

    const char *what() const throw();

  private:
    std::string userMessage;
};

#endif
