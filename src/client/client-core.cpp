#include "client-core.h"

TCPSocket *temp = NULL; // horror^n!

void send_message(char* msg) // horror^n
{
    if(temp)
        temp->send(msg, strlen(msg));
}

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
        temp=&client;
        while(1); // gestore comunicazione
    }
    catch(...)
    {
        cout << "connection attempt on " << CFG_GET_STRING("server_host") << ":" << CFG_GET_INT("server_port") << endl;
    }

    //cout << "connected " << CFG_GET_STRING("server_host") << ":" << CFG_GET_INT("server_port") << endl;
    

    //if (arg)
    //    delete arg;

    pthread_exit(NULL);
}
