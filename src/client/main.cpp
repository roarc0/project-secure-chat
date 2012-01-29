#include <iostream>

#include "../shared/networking/socket.h"
#include "../shared/threading/thread.h"
#include "config-client.h"
#include "gui.h"

using namespace std;

struct session_thread_params
{
    TCPSocket   *sock;
    //?
};

void *session_thread(void *arg)
{
    //if(!t_param)
    //    pthread_exit(NULL);

    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    
    try
    {
        TCPSocket client(CFG_GET_STRING("server_host"), CFG_GET_INT("server_port"));
    }
    catch(...)
    {
        cout << "connection attempt on " << CFG_GET_STRING("server_host") << ":" << CFG_GET_INT("server_port") << endl;
    }

    //cout << "connected " << CFG_GET_STRING("server_host") << ":" << CFG_GET_INT("server_port") << endl;
    while(1); // gestore comunicazione

    //if (t_param)
    //    delete t_param;

    pthread_exit(NULL);
}


int main(int argc, char** argv)
{
    welcome();

    init_config("pschat-client.conf");

    start_thread(&session_thread, (void*)NULL);
    main_gui(argc,argv);

    return 0;
}
