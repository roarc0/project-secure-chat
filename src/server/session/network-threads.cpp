#include "network-threads.h"

void* net_thread(void* arg)
{
    net_thread_params* t_param = (net_thread_params*) arg;

    if(!t_param)
        pthread_exit(NULL);
    
    UserSession      *usession = NULL;
    Packet           *pack;
    
    while (1)
    {     
        usession = s_manager->getNextSessionToServe();
        pack = usession->GetPacketFromSend();
        
        switch (usession->GetSecurity()) 
        {     
        // decripta
        // o effettua il login
        }
        // mette in coda per il thread di esecuzione
    }

    if (t_param)
        delete t_param;

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

void network_threads::start_net_threads(uint32 n)
{
    for (uint32 i = 0 ; i<n ; i++)
        start_net_thread();
}

void network_threads::start_net_thread()
{
    pthread_t      tid;
    net_thread_params* t_params = new net_thread_params;

    tid = start_thread(&net_thread, t_params);

    tids.push_back(tid);
    INFO("verbose", "* net thread %d started \n", tid);
}
