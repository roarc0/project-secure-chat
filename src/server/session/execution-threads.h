#ifndef _EXECUTION_THREADS_H
#define _EXECUTION_THREADS_H

#include "../../shared/execution/command-manager.h"
#include "../../shared/threading/thread.h"
#include "../../shared/common.h"
#include "session-manager.h"

struct exec_thread_params
{
    void    *dummy;
};

void *exec_thread(void *params);

class ExecutionThreads : public Thread
{
  public:
    ExecutionThreads();
    ~ExecutionThreads();

    uint32  Count() const
    {
        return tids.size();
    }

  private:
    vector<pthread_t> tids;
};

#endif
