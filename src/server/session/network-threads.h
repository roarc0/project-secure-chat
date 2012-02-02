#ifndef NETWORK_THREADS_H
#define NETWORK_THREADS_H

#include "../../shared/execution_threads/command-manager.h"
#include "../../shared/threading/thread.h"
#include "../../shared/common.h"
#include "session-manager.h"

struct net_thread_params
{
    command_manager   *c_manager;
    SessionManager    *s_manager;
};

void* net_thread(void*);

class network_threads
{
  public:
    network_threads();
    ~network_threads();

    void    start_net_threads(command_manager*, SessionManager*, uint32);
    void    start_net_thread(command_manager*, SessionManager*);

  private:
    vector<pthread_t> tids;
};

#endif


