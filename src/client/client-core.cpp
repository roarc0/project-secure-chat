#include "client-core.h"

int ClientCore::StartThread(Session *sc)
{
    int ret;
    pthread_t tid;
    pthread_attr_t tattr;

    ret = pthread_attr_init(&tattr);
    if (ret != 0)
        return ret;
    ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    if (ret != 0)
        return ret;
    ret = pthread_create(&tid, &tattr, CoreThread, sc);
    if (ret != 0)
        return ret;

    ret = pthread_attr_destroy(&tattr);
    return ret;
}

void* CoreThread(void* arg)
{
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    Session* session = (Session*)arg;

    INFO("debug","* Receive thread loaded\n");

    while(1)
    {
        try
        {
            while(c_core->IsConnected())
            {
                c_core->HandleRecv();
                msleep(1); 
            }
        }
        catch(SocketException &e)
        {
            INFO("debug", "receive failed (%s:%d) %s\n", // leggere dal socket
                 CFG_GET_STRING("server_host").c_str(),
                 CFG_GET_INT("server_port"), e.what());
            session->SetConnected(false);
        }
        c_core->WaitConnection();
        INFO("debug","* Starting receive loop!\n");
 
    }
    pthread_exit(NULL);
}

ClientCore::ClientCore()
{
    pthread_cond_init (&cond_connection, NULL);
    pthread_mutex_init (&mutex_connection, NULL);
    session = new Session();
    StartThread (session);
}

ClientCore::~ClientCore()
{
    pthread_cond_destroy (&cond_connection);
    pthread_mutex_destroy (&mutex_connection);
}

bool ClientCore::Connect()
{
    bool ret = session->Connect();
    if (ret)
        SignalConnection();
    return ret;
}

bool ClientCore::Disconnect()
{
    return session->Disconnect();
}

bool ClientCore::HandleSend(const char* msg)  // comunicazione in ingresso dall'utente
{
    assert(session && session->IsConnected() && msg);

    INFO("debug","sending message: %s\n", msg);

    Packet pack(0x001);
    pack << msg;
    session->SendPacketToSocket(&pack);

    return true;
}

void ClientCore::HandleRecv()
{
    //Packet pack;
    //session->RecvPacketFromSocket(&pack);
 
    INFO("debug","waiting for data...\n");
 
    Packet *pack;
    pack = session->RecvPacketFromSocket();
    
    INFO("debug","packet received...\n");
 
    //char data[512]="";
    //*pack >> data;
    //INFO("debug", "received message: \"%s\"", data);

    // roba buffa
}
