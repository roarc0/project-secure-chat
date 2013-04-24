#ifndef CORE_CLIENT_H
#define CORE_CLIENT_H

#include "common.h"
#include "utility/singleton.h"
#include "threading/thread.h"
#include "session/session.h"
#include "utility/queues/lock_queue.h"
#include <vector>
#include <sstream>

using namespace std;

enum MessageType
{
    
};

struct Message_t
{
    string data;
    string nick;
    char   type;
    bool   timestamp;
    
    Message_t()
    {
        nick = "user";
    }
    
    Message_t(const std::string& d, char t, bool time)
    {
        data = d;
        type = t;
        timestamp = time;
        nick = "user";
    }
};

void* CoreThread(void*);

class ClientCore
{
    Session* session;

    pthread_t tid;

    pthread_cond_t  cond_connection;
    pthread_mutex_t mutex_connection;

    pthread_cond_t  cond_event;
    pthread_mutex_t mutex_event;

    ClientCore();

  public:

    ~ClientCore();

    bool Connect();
    bool Disconnect();

    bool HandleSend(const char*);
    void HandleRecv();

    int  StartThread(Session *sc);

    bool EmptyEvents();
    Message_t GetEvent();
    void WaitEvent();
    void SignalEvent();
    void WaitConnection();
    void SignalConnection();
    bool IsConnected();

    void AddMessage(std::string& str, char type, bool timestamp = true);

    friend class Singleton<ClientCore>;
    
    //private:
    LockQueue<Message_t> messages;
};

#define c_core  Singleton<ClientCore>::GetInstance()

#endif
