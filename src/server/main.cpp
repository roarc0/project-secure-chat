#include <iostream>

#include "../shared/networking/socket.h"
#include "../shared/threading/thread.h"
#include "config-server.h"

using namespace std;

struct session_thread_params
{
    TCPSocket   *sock;
    //?
};

void *session_thread(void *arg)
{
    session_thread_params* t_param = (session_thread_params*) arg;
    
    if(!t_param)
        pthread_exit(NULL);

    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    
    TCPSocket *sock = t_param->sock;
    
    while(1); // gestore comunicazione

    if (t_param)
        delete t_param;

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    welcome();

    init_config("pschat-server.conf");

    TCPServerSocket server(CFG_GET_INT("server_port"), 128);
    TCPSocket *temp = NULL;
    cout << "listening on port: " << CFG_GET_INT("server_port") << endl;

    while(1)
    {
        temp = server.accept();
        cout << "client connection!" << endl;

        session_thread_params *t_params = new session_thread_params;
        t_params->sock = temp;
        start_thread(&session_thread, (void*)t_params);
    }

    return 0;
}
