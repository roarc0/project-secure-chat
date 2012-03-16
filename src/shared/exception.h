#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>
#include <string.h>
#include <errno.h>

using namespace std;

class Exception : public exception
{
  public:
    Exception(const string &message, bool perr = false) throw();
    ~Exception() throw();

    const char *what() const throw();

  private:
    string uMessage;
};

#endif
