#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include "command.h"

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
    list<command*>  commands;

    string get_opcode(string raw);
    string get_params(string raw);

  public:
    command_manager();
    ~command_manager();

    void       add_command(string id, handler hnd);
    bool       execute(string raw, UserSession *u_session);

};
#endif
