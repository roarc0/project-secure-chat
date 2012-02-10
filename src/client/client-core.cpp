#include "client-core.h"

TCPSocket *csock = NULL;

bool  connect()
{
    if(!csock)
        return false;

    try
    {
        csock->connect(CFG_GET_STRING("server_host"), CFG_GET_INT("server_port"));
    }
    catch(SocketException &e)
    {
        INFO("debug", "[connection]    failed  %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
        return false;
    }

    INFO("debug", "[connection]       success %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));

    return true;
}

bool  disconnect()
{
    if(!csock)
        return false;

    try
    {
        csock->disconnect();
    }
    catch(SocketException &e)
    {
        INFO("debug", "[disconnection] failed  %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
        return false;
    }

    INFO("debug", "[disconnection]    success %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));

    return true;
}

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
        INFO("debug", "[connection]       success %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));
        csock=&client;
    }
    catch(SocketException &e)
    {
        INFO("debug", "[disconnection] failed  %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
    }

    while(1); // gestore comunicazione in uscita

    pthread_exit(NULL);
}
