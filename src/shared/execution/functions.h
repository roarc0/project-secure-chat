#include "../common.h"
#include "command.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// qui ci staranno le funzioni generiche in comune tra client/server
// client e server avranno altre funzioni specifiche in functions-[client,server]

class function_exception : public exception
{
    public:
        function_exception(const std::string &message) throw();
        ~function_exception() throw();

        const char *what() const throw();

    private:
        string user_message;
};


bool dummy(void*);
bool ping(void*);
bool pong(void*);

#endif
