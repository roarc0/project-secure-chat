#include "command-manager.h"

command-manager::command-manager(string id, handler hnd)
{
    cmd_id = id;
    command::hnd = hnd;
}

command-manager::~command-manager()
{

}
