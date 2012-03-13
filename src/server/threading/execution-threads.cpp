#include "execution-threads.h"
#include "method-request.h"

void *exec_thread(void *arg)
{
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    INFO("debug", "* exec thread %d started \n", pthread_self());

    while(1)
    {
        MethodRequest* meth = s_sched_engine->GetNextMethod();
        if (!meth)
            continue;

        meth->call();

        delete meth;
    }

    pthread_exit(NULL);
}

execution_threads::execution_threads()
{

}

execution_threads::~execution_threads()
{
    //for (int i = 0; i < tids.size(); i++)
    //    pthread_join(tids[i], &status);
}

void execution_threads::start_exec_threads(uint32 n)
{
    for (uint32 i = 0 ; i<n ; i++)
        start_exec_thread();
}

void execution_threads::start_exec_thread()
{
    pthread_t      tid;
    exec_thread_params* t_params = new exec_thread_params;

    tid = start_thread(&exec_thread, t_params);
    tids.push_back(tid);
}
