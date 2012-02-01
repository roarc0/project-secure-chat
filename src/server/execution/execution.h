#ifndef EXECUTION_H
#define EXECUTION_H

#include "../../shared/execution/command-manager.h"
#include "../../shared/threading/thread.h"
#include "../../shared/common.h"
#include "../session/session-manager.h"

struct exec_thread_params
{
    command_manager   *c_manager;
    SessionManager    *s_manager;
};

void *exec_thread(void *params);

class execution
{
  public:
    execution();
    ~execution();

    void    start_exec_threads(command_manager*, SessionManager*, uint32);
    void    start_exec_thread(command_manager*, SessionManager*);

  private:
    vector<pthread_t> tids;
};

#endif
