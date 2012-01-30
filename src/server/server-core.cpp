#include "server-core.h"

void* core_thread(void* arg)
{
    core_thread_params* t_param = (core_thread_params*) arg;

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

void start_server_core()
{
    TCPServerSocket server(CFG_GET_INT("server_port"), 128);
    TCPSocket *temp = NULL;
    cout << "listening on port: " << CFG_GET_INT("server_port") << endl;

    while(1)
    {
        temp = server.accept();
        cout << "client connection!" << endl;

        core_thread_params *t_params = new core_thread_params;
        t_params->sock = temp;
        start_thread(&core_thread, (void*)t_params);
    }
}
