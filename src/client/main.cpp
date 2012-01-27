#include <iostream>

#include "../shared/networks/socket.h"
#include "config-client.h"

config *conf = NULL;

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    conf = new config;
    conf->load_args(argc, argv);
    
    TCPSocket client(conf->get_string(CONFIG_SERVER_HOST), conf->get_int(CONFIG_SERVER_PORT));

    return 0;
}
