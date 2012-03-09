#ifndef _COMMAND_MANAGER_H
#define _COMMAND_MANAGER_H

#include "command.h"
#include "functions.h"
#include "../utility/singleton.h"

#define c_manager          CommandManager::GetInstance()

enum cmd_ids // TODO da rivedere
{
    CMD_VOID 0,
    CMD_MAX_SHARED_COMMANDS
};

class CommandManager_exception : public exception
{
    public:
        CommandManagerException(const std::string &message) throw();
        ~CommandManagerException() throw();

        const char *what() const throw();

    private:
        std::string umessage;
};

class CommandManager : public Singleton
{
    list<Command*>  commands;  // trasformare in map

    string GetMessageType(string raw);
    string GetMessageParams(string raw);

    CommandManager();

  public:

    ~CommandManager();

    void   AddCommand(uint32_t, string id, handler hnd);
    bool   Execute(string raw, Session *session);
};
#endif
