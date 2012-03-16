#include "command-manager.h"

CommandManager::CommandManager()
{
    //AddCommand("\\dummy", &Dummy);
    //AddCommand("\\ping",  &Ping);
    //AddCommand("\\pong",  &Pong);
}

void CommandManager::AddCommand(uint32_t id, std::string name, handler hnd)
{
    Command *cmd = new Command(id, name, hnd);
    commands.push_back(cmd);
}


CommandManager::~CommandManager()
{
    for (list<Command*>::iterator itr = commands.begin(); itr != commands.end(); ++itr)
        delete (*itr);
}

int CommandManager::GetMessageType(string raw)
{
    /*size_t pos;
    pos = raw.find_first_of(" ");
    return raw.substr(0, pos);*/
    return 0;
}

string CommandManager::GetMessageParams(string raw)
{
    size_t pos;
    pos = raw.find_first_of(" ");
    return raw.substr(pos + 1);
}

bool CommandManager::Execute(string raw, SessionBase *session) // se non trova il messaggio
{
    list<Command*>::iterator itr = commands.begin();

    for (; itr != commands.end(); ++itr)
        if ((*itr)->GetId() == GetMessageType(raw))
            return (*itr)->Execute(GetMessageParams(raw), session);

    INFO("debug","* command not found!\n");
    return false;
}
