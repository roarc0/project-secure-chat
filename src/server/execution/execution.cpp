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
    SessionManager   *s_manager = t_param->s_manager;
    UserSession      *temp_session = NULL;
    Packet           *pack;

    while(1)
    {
        temp_session = s_manager->getNextSessionToExecute();
        raw          = temp_session->GetPacketFromRecv();
        c_manager->exec(pack->m_data);
        sess->release_session();
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

execution::start_thread(command-manager *c_manager, SessionManager *s_manager)
{
    pthread_t      tid;

    exec_thread_params* t_params = new exec_thread_params;
    t_params->c_manager = c_manager;
    t_params->s_manager = s_manager;

    tid = start_thread(&exec_thread, t_params);

    tids.push_back(tid);
    INFO("verbose", "* exec thread %d started \n", tid);
}
