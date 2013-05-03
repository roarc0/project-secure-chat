#include "client-core.h"
#include "networking/opcode.h"
#include "threading/thread.h"

void* CoreThread(void* arg)
{
    Session* session = (Session*)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    INFO("debug","CLIENT_CORE: receiver thread started\n");

    try
    {
        while(session->IsConnected())
        {
            c_core->HandleRecv();
            msleep(1);
        }
    }
    catch(SocketException &e)
    {
        INFO("debug", "CLIENT_CORE: recv failed (%s:%d) %s\n", // read from socket
             CFG_GET_STRING("server_host").c_str(),
             CFG_GET_INT("server_port"), e.what());
        session->SetConnected(false);
        session->ResetSocket();
        c_core->SignalEvent(); // disconnection Event on error
    }

    pthread_exit(NULL);
}

ClientCore::ClientCore()
{
    pthread_cond_init (&cond_event, NULL);
    pthread_mutex_init (&mutex_event, NULL);

    session = new Session();
}

ClientCore::~ClientCore()
{
    pthread_cond_destroy (&cond_event);
    pthread_mutex_destroy (&mutex_event);
}

bool ClientCore::Connect()
{
    bool ret = session->Connect();
    
    if (ret)
        StartThread(CoreThread, (void*)session, tid);
    
    SignalEvent();
    
    return ret;
}

bool ClientCore::Disconnect()
{
    bool ret = session->Disconnect();
    
    if (ret)
    {   
        pthread_cancel(tid);
        pthread_join(tid,NULL);
        //pthread_kill(tid,SIGQUIT);
        session->ResetSocket();
    }
    
    SignalEvent();
    
    return ret;
}

bool ClientCore::HandleSend(const char* msg)
{
    return session->HandleSend(msg);
}

void ClientCore::HandleRecv()
{
    session->Update();    
}

bool ClientCore::EmptyEvents()
{
    return messages.empty();
}

Message_t ClientCore::GetEvent()
{
    Message_t msg;
    messages.next(msg);
    return msg;
}

void ClientCore::AddMessage(const std::string& str, const string& user,
                            char type, bool timestamp)
{
    Message_t msg;
    std::stringstream ss;
   
    if (timestamp)
        ss << "(" << get_timestamp(':') << ") ";
    if (!user.empty())
        ss << user << ": ";
    ss << str;
    const std::string& tmp = ss.str();
    const char* cstr = tmp.c_str();
    msg.data = cstr;
    msg.user = user;
    msg.type = type;
    msg.timestamp = timestamp;
    messages.add(msg);
}

void ClientCore::WaitEvent()
{
    pthread_cond_wait(&cond_event, &mutex_event);
}

void ClientCore::SignalEvent()
{
    pthread_cond_signal(&cond_event);
}
