#include <iostream>

#include "../shared/networks/socket.h"
#include "config-server.h"

config *conf = NULL;

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    conf = new config;
    conf->load_args(argc, argv);

    TCPServerSocket server(6666, 128);

    return 0;
}
