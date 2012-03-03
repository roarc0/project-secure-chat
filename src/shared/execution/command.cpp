#include "command.h"

command::command(string id, handler hnd)
{
    cmd_id = id;
    command::hnd = hnd;
}

command::~command()
{

}

bool command::execute(string params, UserSession *usession)  // TODO try catch
{
    if(!usession)
        return false;

    SocketServer *sock = usession->GetSocket();

    if(!sock)
        return false;

    handler_params hnd_params(usession, params);

    cout << "* <" << cmd_id << "> executed" << endl;
    //cout << "* <" << cmd_id << "> executed from " << sock->getForeignAddress() << ":" << sock->getForeignPort() << endl;

    return (*hnd)((void*)&hnd_params);
}
