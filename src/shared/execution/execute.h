#ifndef EXECUTE_H
#define EXECUTE_H

#include "../common.h"

struct thread_params
{
    sessions   *sess;
    execute    *exec;
    pthread_t  tid;
};

bool exec_thread(void *params);

class execute
{
  public:
    execute(int);
    ~execute();
    
    exec(string);
    
    start_threads();
    start_thread();
    count_threads() const;
    
    parse_message();

  private:
    vector<pthread_t> tids;
    
};

#endif
