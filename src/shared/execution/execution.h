#ifndef EXECUTION_H
#define EXECUTION_H

#include "../common.h"
#include "../threading/thread.h"

struct exec_thread_params
{
    command-manager   *c_manager;
    session-manager   *s_manager;
    execute           *exec;
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
