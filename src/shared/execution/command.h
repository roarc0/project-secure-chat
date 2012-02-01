#ifndef COMMAND_H
#define COMMAND_H

#include "../common.h"
#include "../networking/user-session.h"

struct handler_params
{
    UserSession *ss;
    string  params;

    command_params(UserSession *u_session, string c_params)
    {
        ss = u_session;
        params = c_params;
    }
};

class command-exception : public exception 
{
    public:
        command-exception(const std::string &message) throw();
        ~command-exception() throw();

        const char *what() const throw();

    private:
        string user_message;
};


class command
{
    string      cmd_id;
    handler     hnd;

  public:

    command(string id, handler hnd);
    ~command();

    bool execute(UserSession*);

    inline string get_id() const
    {
        return cmd_id;
    }
};

#endif
