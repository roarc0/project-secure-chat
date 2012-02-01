#ifndef EXECUTION_H
#define EXECUTION_H

#include "../shared/execution/command-manager.h"
#include "../shared/common.h"
#include "../shared/networking/session-manager.h"
#include "../shared/threading/thread.h"

struct exec_thread_params
{
    command-manager   *c_manager;
    SessionManager    *s_manager;
};

void *exec_thread(void *params);

class execution
{
  public:
    execution(int);
    ~execution();

    start_threads();
    start_thread();
    count_threads() const;

  private:
    vector<pthread_t> tids;
};

#endif
