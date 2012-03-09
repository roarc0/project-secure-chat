#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

#include "../common.h"
#include "command.h"

class FunctionException : public exception
{
    public:
        FunctionException(const std::string &message) throw();
        ~FunctionException() throw();

        const char *what() const throw();

    private:
        string umessage;
};


bool Dummy(void*);
bool Ping(void*);
bool Pong(void*);

#endif
