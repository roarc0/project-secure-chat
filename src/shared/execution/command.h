#ifndef _COMMAND_H
#define _COMMAND_H

#include "../common.h"
#include "../session/sessionbase.h"

// Da rimuovere
struct UserSession
{
};

struct handler_params
{
    UserSession *usession;
    string  params;

    handler_params(UserSession *u_session, string u_params)
    {
        usession = u_session;
        params = u_params;
    }
};

class CommandException : public exception // todo definire
{
    public:
        CommandException(const std::string &message) throw();
        ~CommandException() throw();

        const char *what() const throw();

    private:
        string umessage;
};


class Command // gestire delete
{
    handler     hnd;
    uint32_t    id;
    string      name;


  public:

    Command(uint32_t, string, handler);

    ~Command();

    bool Execute(string params, SessionBase*);

    inline uint32_t GetId() const
    {
        return id;
    }
};

#endif
