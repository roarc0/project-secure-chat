#include "command.h"

command::command(string id, handler hnd)
{
    cmd_id = id;
    command::hnd = hnd;
}

command::~command()
{

}

bool command::execute(UserSession *ss, string params)  // TODO try catch
{
    handler_params hnd_params(ss, params);

    cout << "* <" << id << "> executed from " << ss->get_ip() << ":" << ss->get_port() << endl;

    return (*hnd)((void*)&hnd_params);
}
