#include "client-core.h"

struct core_thread_params
{
    int foo;
    // puntatori e mutex per l'accesso alla grafica gtk
};

void* core_thread(void* arg)
{
    //if(!arg)
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

    //if (arg)
    //    delete arg;

    pthread_exit(NULL);
}
