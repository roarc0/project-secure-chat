#include "client-core.h"
#include "networking/opcode.h"
#include "chat-handler.h"

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
    Session* session = (Session*)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    INFO("debug","* Receive thread loaded\n");

    while(1)
    {
        try
        {
            while(c_core->IsConnected())
            {
                INFO("debug", "inizio RECV\n");
                c_core->HandleRecv();
                INFO("debug", "fine RECV\n");
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
            c_core->SignalMessage(); // disconnection message on error
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
    pthread_cond_init (&cond_message, NULL);
    pthread_mutex_init (&mutex_message, NULL);

    session = new Session();

    if (CFG_GET_BOOL("autoconnect"))
    {
        Connect();
        INFO("debug","autoconnetting...\n");
    }
}

ClientCore::~ClientCore()
{
    pthread_cond_destroy (&cond_connection);
    pthread_mutex_destroy (&mutex_connection);

    pthread_cond_destroy (&cond_message);
    pthread_mutex_destroy (&mutex_message);
}

bool ClientCore::Connect()
{
    bool ret = session->Connect();

    if (ret)
    {
        StartThread(session);
        SignalMessage();
        //SignalConnection();
    }
    return ret;
}

bool ClientCore::Disconnect()
{
    bool ret = session->Disconnect();
    if (ret)
    {
        SignalMessage();
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
    //stringstream ss;
    //ss << CFG_GET_STRING("nickname") << " " << msg;
    
    if (!session->IsConnected())
        return false;

    INFO("debug","sending message: %s\n", msg);

    if (ChatHandler(session).ParseCommands(msg) > 0)
        return true;

    Packet pack(CMSG_MESSAGE);
    pack << xmsg.BuildMessage(CFG_GET_STRING("nickname").c_str(), msg);
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

    
    messages.add(xmsg.ReadMessage(str.c_str()));
    SignalMessage();
}

string ClientCore::GetMessage()
{
    string ev;
    messages.next(ev);
    return ev;
}

void ClientCore::WaitMessage()
{
    pthread_cond_wait(&cond_message, &mutex_message);
}

void ClientCore::SignalMessage()
{
    pthread_cond_signal(&cond_message);
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
