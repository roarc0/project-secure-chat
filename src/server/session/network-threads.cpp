#include "network-threads.h"

void* net_thread(void* arg)
{
    net_thread_params* t_param = (net_thread_params*) arg;

    //if(!t_param)
    //    pthread_exit(NULL);
        
    //SessionManager* s = t_param->sessions;
    while (1)
     {
        //s->getNextSessionToServe();
        // verifica se ci sono pacchetti in arrivo
        // decripta
        // mette in coda per il thread di esecuzione
    }

    //if (t_param)
    //    delete t_param;

    pthread_exit(NULL);
}


network_threads::network_threads()
{

}

network_threads::~network_threads()
{
    //for (int i = 0; i < tids.size(); i++)
    //    pthread_join(tids[i], &status);
}

void network_threads::start_net_threads(SessionManager *s_manager, uint32 n)
{
    for (uint32 i = 0 ; i<n ; i++)
        start_net_thread(s_manager);
}

void network_threads::start_net_thread(SessionManager *s_manager)
{
    pthread_t      tid;

    net_thread_params* t_params = new net_thread_params;
    //t_params->c_manager = c_manager;
    //t_params->s_manager = s_manager;

    tid = start_thread(&net_thread, t_params);

    tids.push_back(tid);
    INFO("verbose", "* net thread %d started \n", tid);
}
