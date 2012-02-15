#ifndef COMMAND_H
#define COMMAND_H

#include "../common.h"
#include "../session/user-session.h"

struct handler_params
{
    UserSession *ss;
    string  params;

    handler_params(UserSession *usession, string uparams)
    {
        ss = usession;
        params = uparams;
    }
};

class command_exception : public exception
{
    public:
        command_exception(const std::string &message) throw();
        ~command_exception() throw();

        const char *what() const throw();

    private:
        string u_message;
};


class command // gestire delete
{
    string      cmd_id;
    handler     hnd;

  public:

    command(string id, handler hnd);
    ~command();

    bool execute(string params, UserSession*);

    inline string get_id() const
    {
        return cmd_id;
    }
};

#endif
