#ifndef NETWORK_THREADS_H
#define NETWORK_THREADS_H

#include "../../shared/execution/command-manager.h"
#include "../../shared/threading/thread.h"
#include "session-manager.h"

struct net_thread_params
{
    void    *dummy;
};

void* net_thread(void*);

class network_threads
{
  public:
    network_threads();
    ~network_threads();

    void    start_net_threads(uint32);
    void    start_net_thread();

  private:
    vector<pthread_t> tids;
};

#endif


