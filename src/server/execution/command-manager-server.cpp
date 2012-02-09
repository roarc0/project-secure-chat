#include "command-manager-server.h"

void init_commands()
{
    c_manager->add_command("send", send_message);
    //c_manager->add_command("login", &login(void*));
    //c_manager->add_command("logout", &logout(void*));
    //c_manager->add_command("join", &join(void*));
}
