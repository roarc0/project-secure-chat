#include <iostream>

#include "../shared/networks/socket.h"
#include "config-client.h"
#include "gui.h"

config *conf = NULL;

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    conf = new config;
    conf->load_args(argc, argv);

    try
    {
        TCPSocket client(conf->get_string(CONFIG_SERVER_HOST), conf->get_int(CONFIG_SERVER_PORT));
    }
    catch(...)
    {
        cout << "test" << endl;
    }
    
    main_gui(argc,argv);

    return 0;
}
