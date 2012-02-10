#include "client-core.h"

TCPSocket *csock = NULL;

void handle_message(char* msg)  // comunicazione in ingresso dall'utente
{
    if(!csock)
        return;

    
    csock->send(msg, strlen(msg));
}

struct core_thread_params
{
    int foo;
    // puntatori e mutex per l'accesso alla grafica gtk
};

void* core_thread(void* arg)
{
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    try
    {
        TCPSocket client(CFG_GET_STRING("server_host"), CFG_GET_INT("server_port"));
        INFO("debug", "connected on %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));
        csock=&client;
        while(1); // gestore comunicazione in uscita
    }
    catch(...)
    {
        INFO("debug", "connection failed on %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));
    }

    pthread_exit(NULL);
}
