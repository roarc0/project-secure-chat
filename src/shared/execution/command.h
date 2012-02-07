#ifndef COMMAND_H
#define COMMAND_H

#include "../common.h"
#include "../session/user-session.h"

struct handler_params
{
    UserSession *ss;
    string  params;

    handler_params(UserSession *u_session, string c_params)
    {
        ss = u_session;
        params = c_params;
    }
};

class command_exception : public exception
{
    public:
        command_exception(const std::string &message) throw();
        ~command_exception() throw();

        const char *what() const throw();

    private:
        string user_message;
};


class command // gestire delete
{
    string      cmd_id;
    handler     hnd;

  public:

    command(string id, handler hnd);
    ~command();

    bool execute(UserSession*, string params);

    inline string get_id() const
    {
        return cmd_id;
    }
};

#endif
