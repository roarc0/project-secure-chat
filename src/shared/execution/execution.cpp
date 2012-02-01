#include "execution.h"

void *exec_thread(void *params)
{
    exec_thread_params* t_param = (exec_thread_params*) arg;
    
    if(!t_param)
        pthread_exit(NULL);

    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    command-manager  *c_manager = t_param->c_manager;
    session-manager  *s_manager = t_param->s_manager;
    session          *temp_session = NULL;
    string           raw_plaintext;

    while(1) // while condizione morte thread ... per ora non muore mai
    {
        temp_session = sess->get_session();
        raw_plaintext = temp_session->get_message();
        exec->exec(message);
        sess->release_exec_pointer();
    }
    
    if (t_param)
        delete t_param;

    pthread_exit(NULL);

}

execution::execution()
{

}

execution::~execution()
{
    //for (int i = 0; i < tids.size(); i++)
    //    pthread_join(tids[i], &status);
}

execution::start_threads(int n)
{
    for (int i = 0 ; i<n ; i++)
        start_thread();
}

execution::start_thread()
{
    pthread_t      tid;

    exec_thread_params* t_params = new exec_thread_params;
    t_params->c_manager = this;
    t_params->sess = session_pointer;

    tid = start_thread(&exec_thread, t_params);

    tids.push_back(tid);
    INFO("verbose", "* exec thread %d started \n", tid);
}
