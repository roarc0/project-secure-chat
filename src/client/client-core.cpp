#include "client-core.h"

client_core* client_core::ccore_singleton = NULL;
TCPSocket *csock = NULL;

void* core_thread(void* arg)
{
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    while(1)
    try
    {
        TCPSocket client(CFG_GET_STRING("server_host"), CFG_GET_INT("server_port"), false);
        INFO("debug", "connection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));
        c_core->set_connected(true);

        csock=&client;

        while(c_core->is_connected())
        {
            msleep(1000); // gestore comunicazione in uscita
        }
        c_core->set_connected(false);
        msleep(6000);   // segnale di wait, quando si clicca su connect si fa il signal
        INFO("debug", "restarting client connection\n");
    }
    catch(SocketException &e)
    {
        INFO("debug", "connection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
    }

    pthread_exit(NULL);
}

client_core::client_core()
{
    connected = false;
    start_thread(&core_thread, (void*)NULL);
    whoami = "client";
    dummy(NULL);
}

bool client_core::connect()
{
    if(!csock)
        return false;

    try
    {
        csock->connect(CFG_GET_STRING("server_host"), CFG_GET_INT("server_port"));
        c_core->set_connected(true);
    }
    catch(SocketException &e)
    {
        c_core->set_connected(false);
        INFO("debug", "connection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
        return false;
    }

    INFO("debug", "connection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));

    return true;
}

bool client_core::disconnect()
{
    if(!csock)
        return false;

    try
    {
        csock->disconnect();
        c_core->set_connected(false);
    }
    catch(SocketException &e)
    {
        INFO("debug", "disconnection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
        return false;
    }

    INFO("debug", "disconnection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));

    return true;
}

void client_core::handle_message(const char* msg)  // comunicazione in ingresso dall'utente
{
    if(!csock || !msg)
        return;

    Packet pack = ForgePacket(OP_NULL, msg);
    unsigned char* rawData = new unsigned char[pack.GetRawLength() + 1];
    pack.GetRawData(rawData);
    csock->send(rawData, pack.GetRawLength());
    delete[] rawData;
}
