#ifndef EXECUTION_THREADS_H
#define EXECUTION_THREADS_H

#include "../../shared/execution/command-manager.h"
#include "../../shared/threading/thread.h"
#include "../../shared/common.h"
#include "session-manager.h"

struct exec_thread_params
{
    SessionManager    *s_manager;
};

void *exec_thread(void *params);

class execution_threads
{
  public:
    execution_threads();
    ~execution_threads();

    void    start_exec_threads(SessionManager*, uint32);
    void    start_exec_thread(SessionManager*);

  private:
    vector<pthread_t> tids;
};

#endif
