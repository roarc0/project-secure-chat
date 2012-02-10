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
        INFO("debug", "connection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
        return false;
    }

    INFO("debug", "connection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));

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
        INFO("debug", "connection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
        return false;
    }

    INFO("debug", "connection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));

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
        TCPSocket client(CFG_GET_STRING("server_host"), CFG_GET_INT("server_port"), false);
        INFO("debug", "connection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));
        usleep(1000);
        csock=&client;

        while(1) {
            //client.send("ciao",4);
            usleep(1000);
        } // gestore comunicazione in uscita
    }
    catch(SocketException &e)
    {
        INFO("debug", "connection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
    }



    pthread_exit(NULL);
}
