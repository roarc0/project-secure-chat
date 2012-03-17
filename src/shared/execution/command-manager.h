#ifndef _COMMAND_MANAGER_H
#define _COMMAND_MANAGER_H

#include <list>
#include "command.h"
#include "functions.h"
#include "../singleton.h"
#include "../exception.h"
#include "../session/sessionbase.h"


enum cmd_ids // TODO da rivedere
{
    CMD_VOID = 0,
    CMD_MAX_SHARED_COMMANDS
};

class CommandManager_exception : public Exception {};

class CommandManager
{
    friend class Singleton<CommandManager>;

    private:
        CommandManager();
        ~CommandManager();

        list<Command*>  commands;  // trasformare in map

        int GetMessageType(string raw);
        string GetMessageParams(string raw);  

    public:
        void   AddCommand(uint32_t id, std::string name, handler hnd);
        bool   Execute(string raw, SessionBase *session);
};

#define c_manager Singleton<CommandManager>::GetInstance()
#endif
