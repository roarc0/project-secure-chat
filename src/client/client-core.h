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
    string user;
    char   type;
    bool   timestamp;
    
    Message_t()
    {
        user = "";
    }
    
    Message_t(const std::string& d, char t, bool time)
    {
        data = d;
        type = t;
        timestamp = time;
        user = "";
    }
};

void* CoreThread(void*);

class ClientCore
{
    Session* session;

    pthread_t tid;

    pthread_cond_t  cond_event;
    pthread_mutex_t mutex_event;

    ClientCore();

  public:

    ~ClientCore();

    bool Connect();
    bool Disconnect();

    bool HandleSend(const char*);
    void HandleRecv();

    bool EmptyEvents();
    Message_t GetEvent();
    void WaitEvent();
    void SignalEvent();
    bool IsConnected();
    
    Session* GetSession()
    {
        return session;
    }
    
    const char* GetUsername();
    bool SetUsername(const std::string&);

    void AddMessage(const std::string& str, const std::string& nick,
                    char type, bool timestamp = true);

    friend class Singleton<ClientCore>;
    
    private:
    LockQueue<Message_t> messages;
};

#define c_core  Singleton<ClientCore>::GetInstance()

#endif
