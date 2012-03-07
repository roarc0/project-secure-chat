#include "command.h"

Command::Command(uint32_t _id, string _name, handler _hnd)  :
id(_id), hnd(_hnd), name(_name);
{

}

Command::~Command()
{

}

bool Command::Execute(string params, Session *session)  // TODO try catch
{
    if(!session)
        return false;

    handler_params hnd_params(session, params);

    cout << "* <" << cmd_id << "> executed" << endl;
    //cout << "* <" << cmd_id << "> executed from " << session->sock->getForeignAddress() << ":" << session->sock->getForeignPort() << endl;

    return (*hnd)((void*)&hnd_params);
}
