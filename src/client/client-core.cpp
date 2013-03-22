#include "client-core.h"

void* CoreThread(void* arg)
{
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    SocketClient* csock = (SocketClient*)arg;

    try
    {
        INFO("debug", "connection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));
        c_core->SetConnected(true);

        while(1)
        {
            while(c_core->IsConnected())
            {
                c_core->HandleRecv(); // gestore comunicazione in ingresso
                msleep(3); 
            }
            msleep(300);      // wait, quando si clicca su connect si fa il signal
        }
    }
    catch(SocketException &e)
    {
        INFO("debug", "connection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
    }

    pthread_exit(NULL);
}

ClientCore::ClientCore()
{
    connected = false;
    csock = new SocketClient(SOCK_STREAM, 0);
    //start_thread(&core_thread, (void*)csock);
}

bool ClientCore::Connect()
{
    if(!csock)
        return false;

    try
    {
        csock->Connect(CFG_GET_STRING("server_host"), CFG_GET_INT("server_port"));
        c_core->SetConnected(true);
    }
    catch(SocketException &e)
    {
        c_core->SetConnected(false);
        INFO("debug", "connection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
        return false;
    }

    INFO("debug", "connection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));

    return true;
}

bool ClientCore::Disconnect()
{
    if(!csock)
        return false;

    try
    {
        csock->Disconnect();
        c_core->SetConnected(false);
    }
    catch(SocketException &e)
    {
        INFO("debug", "disconnection failed     %s:%d (%s)\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"), e.what());
        return false;
    }

    INFO("debug", "disconnection successful %s:%d\n", CFG_GET_STRING("server_host").c_str(), CFG_GET_INT("server_port"));

    return true;
}

bool ClientCore::HandleSend(const char* msg)  // comunicazione in ingresso dall'utente
{
    if(!connected || !csock || !msg)
        return false;

    string str_msg;

    if (msg[0] != '\\')
    {
        str_msg = "\\send ";
    }

    str_msg += msg;

    INFO("debug","sending message: %s\n", msg);

    Packet pack(OP_NULL);
    pack<<str_msg;
    
    unsigned char* rawData = pack.GetData();
    //Packet pack = ForgePacket(OP_NULL, str_msg.c_str());
    //unsigned char* rawData = new unsigned char[pack.GetSize() + 1];
    //pack.GetRawData(rawData);
    //csock->Send(rawData, pack.GetSize()); // TODO Check send   
    csock->Send(str_msg.c_str(),strlen(str_msg.c_str()));
    delete[] rawData;
    return true;
}

void ClientCore::HandleRecv()
{
    char *buffer = NULL;
    unsigned short len = 0, opcode;

    try
    {
        if (csock->Recv(&opcode, OPCODE_SIZE) == 0);
            return;
        INFO("debug","opcode : %d\n", opcode);
        csock->Recv(&len, LENGTH_SIZE);
        INFO("debug","len    : %d\n", len);
        buffer = new char[len+1];
        csock->Recv(buffer, len);
        buffer[len] = '\0';
        INFO("debug","msg    : \"%s\"\n", buffer);

        delete buffer;
        buffer = NULL;
    } 
    catch (SocketException e)
    {
        INFO("debug","* client session error, %s\n", e.what());
        if (buffer)
        {
            delete buffer;
            buffer = NULL;
        }
        c_core->SetConnected(false);
        csock->InitSocket();
    }
}
