#include "network-threads.h"
#define NSEND 2
#define NRECV 1


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
        for (int i = NSEND; i--;) 
        {             
            int len;
            char* buffer;
            pack = usession->GetPacketFromSend();
            
            //if (usession->GetSecurity())
                // buffer = cripta (buffer + 6,len)
            //else 
            { 
                len = 6 + pack->m_data.size() +1;
                buffer = new char[len];
                memcpy(buffer, pack->GetOpcodePointer(), 2);
                memcpy(buffer, &len, 4);
                strcpy(buffer + 6, pack->m_data.c_str());
            }            
            
            usession->GetSocket()->send(buffer,len);
        } 
        
        for (int i = NRECV; i--;) 
        { 
            pack = new Packet;
            usession->GetSocket()->recv(pack->GetOpcodePointer(),2);
            int len;
            usession->GetSocket()->recv(&len,4);            
            char* buffer = new char[len+1];
            usession->GetSocket()->recv(buffer,len);
            
            //if (usession->GetSecurity())
                // decripta (buffer,len)                
                
            buffer[len] = '\0';
            pack->m_data = buffer;
            
            usession->QueuePacketToRecv(pack);
        }
        
        //if (s_manager->MoreThreadsThanClients) // i'm useless
        //    break; //harakiri
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
