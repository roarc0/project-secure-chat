#include "command-manager.h"

command-manager::command-manager(string id, handler hnd)
{
    cmd_id = id;
    command::hnd = hnd;
}

command-manager::~command-manager()
{

}

void command-manager::add_command(string id, handler hnd)
{
    command *cmd = new command(id, hnd);
    commands.push_back(cmd);
}

string command-manager::get_opcode(string raw)
{
    size_t pos;
    pos = raw.find_first_of(" ");
    return raw.substr(0, pos);
}

string command-manager::get_params(string raw)
{
    size_t pos;
    pos = raw.find_first_of(" ");
    return raw.substr(pos + 1);
}

bool command-manager::execute(string raw, UserSession *u_session) // se non trova il messaggio
{
    list<command>:const_iterator itr = commands.begin();

    for (; itr != commands.end(); ++itr)
    {
        if(itr != commands.end() && (*itr.get_id() == get_opcode(raw)))
            (*itr).execute(u_session, get_params(raw));
    }
}
