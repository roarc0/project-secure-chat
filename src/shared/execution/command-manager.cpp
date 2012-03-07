#include "command-manager.h"

CommandManager* CommandManager::_instance = NULL;

CommandManager::CommandManager()
{
    AddCommand("\\dummy", &Dummy);
    AddCommand("\\ping",  &Ping);
    AddCommand("\\pong",  &Pong);
}


CommandManager::~CommandManager()
{
    for (list<command*>::iterator itr = commands.begin(); itr != commands.end(); ++itr)
        delete (*itr);
}

void CommandManager::AddCommand(uint32_t id, string name, handler hnd)
{
    Command *cmd = new Command(id, name, hnd);
    commands.push_back(cmd);
}

string CommandManager::GetMessageType(string raw)
{
    size_t pos;
    pos = raw.find_first_of(" ");
    return raw.substr(0, pos);
}

string CommandManager::GetMessageParams(string raw)
{
    size_t pos;
    pos = raw.find_first_of(" ");
    return raw.substr(pos + 1);
}

bool CommandManager::Execute(string raw, Session *session) // se non trova il messaggio
{
    list<command*>::iterator itr = commands.begin();

    for (; itr != commands.end(); ++itr)
        if(itr != commands.end() && (*itr)->GetId() == GetMessageType(raw))
            return (*itr)->execute(GetMessageParams(raw), session);

    INFO("debug","* command not found!\n");
    return false;
}
