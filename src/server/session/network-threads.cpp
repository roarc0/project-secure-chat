#include "network-threads.h"
#include "../../shared/cryptography/crypto.h"

#define   NSEND         2
#define   NRECV         1

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
            char* buffer = NULL;

            pack = usession->GetPacketFromSend();
            if (!pack)
                break;

            //if (usession->GetSecurity())
                // buffer = cripta (buffer + 6,len)
            //else 
            {
                len = pack->GetRawLength();
                buffer = new char[len + 1];
                pack->GetRawData((unsigned char*)buffer);
                buffer[len] = '\0';
            }

            try
            {
                //usession->GetSocket()->send(buffer,len);
            }
            catch (SocketException e)
            {
                delete[] buffer;
                INFO("debug","%s\n", e.what());
            }

            delete[] buffer;
        }
        
        for (int i = NRECV; i--;)
        {
            char* buffer = NULL;
            try 
            {
                unsigned short len = 0;
                pack = new Packet;

                //if (usession->GetSocket()->recv(pack->GetOpcodePointer(), OPCODE_SIZE) == 0)
                {
                    delete pack;
                    break;
                }

                INFO("debug","opcode : %d\n", pack->GetOpcode());
                //if (usession->GetSocket()->recv(&len, LENGTH_SIZE) == 0)
                {
                    delete pack;
                    break;
                }

                INFO("debug","len    : %d\n", len);
                buffer = new char[len+1];
                //if (usession->GetSocket()->recv(buffer, len) == 0)
                {
                    delete pack;
                    break;
                }



                if (len > 1)
                    buffer[len] = '\0';
                INFO("debug","msg    : \"%s\"\n", buffer);

                //if (usession->GetSecurity())
                    // decripta (buffer,len)

                pack->m_data = buffer;
                delete buffer;
                buffer = NULL;

                usession->QueuePacketToRecv(pack);
            } 
            catch (SocketException e)
            {
                INFO("debug","* client session error, %s\n", e.what());
                delete pack;
                if (buffer)
                {
                    delete buffer;
                    buffer = NULL;
                }
                s_manager->deleteSession(usession->GetId());                // UCCIDO LA SESSIONE
            }
        }

        usession->releaselock_net();

        //if (s_manager->MoreThreadsThanClients) // i'm useless
        //    break; //harakiri

        msleep(5);
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
