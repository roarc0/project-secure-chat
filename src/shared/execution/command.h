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
    SessionBase *bsession;
    string  params;

    handler_params(SessionBase *b_session, string u_params)
    {
        bsession = b_session;
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

    Command(uint32_t _id, string _name, handler _hnd);
    ~Command();

    bool Execute(string params, SessionBase*);

    uint32_t GetId() const
    {
        return id;
    }
};

#endif
