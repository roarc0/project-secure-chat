#include "client-core.h"

// LANCIAFIAMME
int ClientCore::StartThread(Session *sc)
{
    int ret;
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
    //sigset_t mask;
    //sigfillset(&mask);
    //pthread_sigmask(SIG_BLOCK, &mask, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

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
            INFO("debug", "* recv failed (%s:%d) %s\n", // read from socket
                 CFG_GET_STRING("server_host").c_str(),
                 CFG_GET_INT("server_port"), e.what());
            session->SetConnected(false);
            session->ResetSocket();
            c_core->SignalEvent(); // disconnection event on error
        }
        c_core->WaitConnection();
        INFO("debug","* receive loop!\n");

    }
    pthread_exit(NULL);
}

ClientCore::ClientCore()
{
    pthread_cond_init (&cond_connection, NULL);
    pthread_mutex_init (&mutex_connection, NULL);

    pthread_cond_init (&cond_event, NULL);
    pthread_mutex_init (&mutex_event, NULL);

    session = new Session();

    if (CFG_GET_BOOL("autoconnect"))
    {
        Connect();
        INFO("debug","autoconnetting...\n");
    }
    //StartThread (session);
}

ClientCore::~ClientCore()
{
    pthread_cond_destroy (&cond_connection);
    pthread_mutex_destroy (&mutex_connection);

    pthread_cond_destroy (&cond_event);
    pthread_mutex_destroy (&mutex_event);
}

bool ClientCore::Connect()
{
    bool ret = session->Connect();

    if (ret)
    {
        StartThread(session);
        SignalEvent();
        SignalConnection();
    }
    return ret;
}

bool ClientCore::Disconnect()
{
    bool ret = session->Disconnect();
    if (ret)
    {
        SignalEvent();
        INFO("debug","thread tritolo pronto\n");
        pthread_cancel(tid);
        pthread_join(tid,NULL);
        INFO("debug","thread esploso\n");
        //pthread_kill(tid,SIGQUIT);
        session->ResetSocket();
    }
    return ret;
}

bool ClientCore::HandleSend(const char* msg)
{
    assert(session && msg);

    if (!session->IsConnected())
        return false;

    INFO("debug","sending message: %s\n", msg);

    Packet pack(0x001);
    pack << msg;
    session->SendPacketToSocket(&pack); // TODO fare sendpacket in session del client

    return true;
}

void ClientCore::HandleRecv()
{
    Packet *pack;
    INFO("debug","* waiting for data...\n");
    pack = session->RecvPacketFromSocket();
    INFO("debug","* packet received...\n");

    string str;
    if(pack)
        *pack >> str;
    else
        INFO("debug", "* empty packet received\n");

    eventg ev;
     //passare il pacchetto analizzato e trasformato in evento
    ev.who="server";
    ev.what="something";
    ev.data=str;
    events.push_back(ev);
    SignalEvent();

}

eventg ClientCore::GetEvent()
{
    eventg ev;
    if(!events.empty())
    {
        ev=events.back();
        events.pop_back();
    }
    return ev;
}

void ClientCore::WaitEvent()
{
    pthread_cond_wait(&cond_event, &mutex_event);
}

void ClientCore::SignalEvent()
{
    pthread_cond_signal(&cond_event);
}

void ClientCore::WaitConnection()
{
    pthread_cond_wait(&cond_connection, &mutex_connection);
}

void ClientCore::SignalConnection()
{
    pthread_cond_signal(&cond_connection);
}

bool ClientCore::IsConnected()
{
    return session->IsConnected();
}
