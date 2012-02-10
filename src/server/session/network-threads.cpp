#include "network-threads.h"
#include "../../shared/cryptography/crypto.h"
#define NSEND 2
#define NRECV 1


void* net_thread(void* arg)
{
    UserSession      *usession = NULL;
    Packet           *pack;

    INFO("debug", "* net thread %d started \n", pthread_self());

    while (1)
    {
        usession = s_manager->getNextSessionToServe();
        if (!usession)
            continue;

        for (int i = NSEND; i--;)
        {
            int len;
            char* buffer;
            pack = usession->GetPacketFromSend();
            if (!pack)
                continue;

            INFO("debug","send message!\n");

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

            try
            {
                usession->GetSocket()->send(buffer,len);
            } 
            catch (SocketException e)
            {
                cout<<e.what();
            }
            delete buffer;
        }
        
        for (int i = NRECV; i--;)
        {
            char* buffer = NULL;
            try 
            {
                pack = new Packet;
                if (usession->GetSocket()->recv(pack->GetOpcodePointer(),2) == -1)
                {
                    delete pack;
                    break; // Niente da leggere nel socket
                }
                int len;
                if (usession->GetSocket()->recv(&len,4) == -1)
                {
                    delete pack;
                    break; // Errore inatteso
                }
                buffer = new char[len+1];
                if (usession->GetSocket()->recv(buffer,len) == -1)
                {
                    delete pack;
                    delete buffer;
                    buffer = NULL;
                    break; // Errore inatteso
                }

                //if (usession->GetSecurity())
                    // decripta (buffer,len)

                buffer[len] = '\0';

                pack->m_data = buffer;
                delete buffer;
                buffer = NULL;

                usession->QueuePacketToRecv(pack);
                INFO("debug","recv message!\n");
            } 
            catch (SocketException e)
            {
                cout<<e.what();
                delete pack;
                if (buffer)
                {
                    delete buffer;
                    buffer = NULL;
                }
            }
            usession->releaselock_net();
        }
        
        //if (s_manager->MoreThreadsThanClients) // i'm useless
        //    break; //harakiri
        usleep(5);
    }

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
}
