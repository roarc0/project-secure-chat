#ifndef EXECUTE_H
#define EXECUTE_H

#include "../common.h"

struct execute_thread_params
{
    sessions   *sess;
    execute    *exec;
    pthread_t  tid;
};

void *exec_thread(void *params);

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
