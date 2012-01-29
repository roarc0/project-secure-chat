#include <iostream>

#include "../shared/networks/socket.h"
#include "config-client.h"
#include "gui.h"

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    init_config("pschat-client.conf");

    try
    {
        TCPSocket client(CFG_GET_STRING("host"), CFG_GET_INT("port"));
    }
    catch(...)
    {
        cout << "connection attempt on " << CFG_GET_STRING("host") << ":" << CFG_GET_INT("port") << endl;
    }
    
    main_gui(argc,argv);

    return 0;
}
