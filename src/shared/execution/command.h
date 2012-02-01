#ifndef COMMAND_H
#define COMMAND_H

#include "common.h"

struct handler_params
{
    session *ss;
    string  params;

    command_params(session *cmd_ss, string cmd_params)
    {
        ss = cmd_ss;
        params = cmd_params;
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

    bool execute(session *ss);

    inline string get_id() const
    {
        return cmd_id;
    }
};

#endif
