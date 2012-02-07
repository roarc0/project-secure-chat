#include "command-manager.h"

command_manager* command_manager::cmd_singleton = NULL;

command_manager::~command_manager()
{
    for (list<command*>::iterator itr = commands.begin(); itr != commands.end(); ++itr)
        delete (*itr);
}

void command_manager::add_command(string id, handler hnd)
{
    command *cmd = new command(id, hnd);
    commands.push_back(cmd);
}

string command_manager::get_opcode(string raw)
{
    size_t pos;
    pos = raw.find_first_of(" ");
    return raw.substr(0, pos);
}

string command_manager::get_params(string raw)
{
    size_t pos;
    pos = raw.find_first_of(" ");
    return raw.substr(pos + 1);
}

bool command_manager::execute(string raw, UserSession *u_session) // se non trova il messaggio
{
    list<command*>::iterator itr = commands.begin();

    for (; itr != commands.end(); ++itr)
    {
        if(itr != commands.end() && (*itr)->get_id() == get_opcode(raw))
            (*itr)->execute(u_session, get_params(raw));
    }
}