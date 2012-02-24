#include "client-core.h"

client_core* client_core::ccore_singleton = NULL;

void* core_thread(void* arg)
{
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    TCPSocket* csock = (TCPSocket*)arg;

    try
    {
        INFO("debug", "connection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));
        c_core->set_connected(true);

        while(1)
        {
            while(c_core->is_connected())
            {
                c_core->handle_recv(); // gestore comunicazione in ingresso
                msleep(5); 
            }
            c_core->set_connected(false);
            msleep(6000);                                          // realizzare un segnale di wait, quando si clicca su connect si fa il signal
            INFO("debug", "restarting client connection\n");
        }
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
    csock = new TCPSocket(CFG_GET_STRING("server_host"), CFG_GET_INT("server_port"), false);
    start_thread(&core_thread, (void*)csock);
    whoami = "client";
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

void client_core::handle_send(const char* msg)  // comunicazione in ingresso dall'utente
{
    if(!csock || !msg)
        return;

    string str_msg;

    if (msg[0] != '\\')
    {
        str_msg = "\\send ";
    }

    str_msg += msg;

    INFO("debug","sending message: %s\n", msg);

    Packet pack = ForgePacket(OP_NULL, str_msg.c_str());
    unsigned char* rawData = new unsigned char[pack.GetRawLength() + 1];
    pack.GetRawData(rawData);
    csock->send(rawData, pack.GetRawLength());
    delete[] rawData;
}

void client_core::handle_recv()
{
    char *buffer = NULL;
    unsigned short len = 0;
    Packet* pack = new Packet;

    try
    {
        csock->recv(pack->GetOpcodePointer(), OPCODE_SIZE);
        INFO("debug","opcode : %d\n", pack->GetOpcode());
        csock->recv(&len, LENGTH_SIZE);
        INFO("debug","len    : %d\n", len);
        buffer = new char[len+1];
        csock->recv(buffer, len);
        buffer[len] = '\0';
        INFO("debug","msg    : \"%s\"\n", buffer);

        //if (usession->GetSecurity())
            // decripta (buffer,len)

        pack->m_data = buffer;
        delete buffer;
        buffer = NULL;
    } 
    catch (SocketException e)
    {
        INFO("debug","* client session error, %s\n", e.what());
        delete pack;
        if (buffer)
        {
            delete buffer;
            buffer = NULL;
        }
    }
}
