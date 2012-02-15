#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include "command.h"
#include "functions.h"

#define c_manager          command_manager::get_instance()

class command_manager_exception : public exception
{
    public:
        command_manager_exception(const std::string &message) throw();
        ~command_manager_exception() throw();

        const char *what() const throw();

    private:
        std::string user_message;
};

class command_manager
{
    list<command*>             commands;
    static command_manager*    cmd_singleton;

    string get_message_type(string raw);
    string get_message_params(string raw);

    command_manager()
    {
        add_command("\\dummy", &dummy);
    }

  public:

    static command_manager* get_instance()
    {
        if(!cmd_singleton)
            cmd_singleton = new command_manager();
        return cmd_singleton;
    };

    ~command_manager();

    void   add_command(string id, handler hnd);
    bool   execute(string raw, UserSession *u_session);
};
#endif
