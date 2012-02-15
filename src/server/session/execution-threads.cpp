#include "execution-threads.h"

void *exec_thread(void *arg)
{
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    UserSession      *usession = NULL;
    Packet           *pack;

    INFO("debug", "* exec thread %d started \n", pthread_self());

    while(1)
    {
        usession = s_manager->getNextSessionToExecute();
        if (!usession)
            continue;

        pack = usession->GetPacketFromRecv();
        if (pack && pack->m_data != "")
        {
            INFO("debug","INCOMING MESSAGE: \"%s\"\n", pack->m_data.c_str());
            c_manager->execute(pack->m_data, usession);
        }

        usession->releaselock_exec();
        usleep(5);
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
