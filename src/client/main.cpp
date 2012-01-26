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
    
    //TCPClientSocket client(6666);

    return 0;
}
