#include "network-manager.h"

class SocketThread: public MethodRequest
{
    private:

        NetworkManager& m_netmanager;
        uint32 m_diff;
        bool active;

    public:

        SocketThread(NetworkManager& netmanager, uint32 d) : 
        MethodRequest(), m_netmanager(netmanager), m_diff(d), active(true)
        {
            
        }

        ~SocketThread()
        {

        }

        int Call()
        { 
            //inizializzazione callback
            while (active)
            {
                msleep(1000);
                // Cose buffe epoll
            }
            return 0;
        }
};

class NetworkThread: public MethodRequest
{
    private:

        NetworkManager& m_netmanager;
        uint32 m_diff;
        bool active;

    public:

        NetworkThread(NetworkManager& netmanager, uint32 d) : 
        MethodRequest(), m_netmanager(netmanager), m_diff(d), active(true)
        {
     
        }

        ~NetworkThread()
        {

        }

        int Call()
        {
            netsession_pair net_ses;
            Packet* pkt = NULL;

            while (active)
            {            
                net_ses = m_netmanager.GetNextSession();
                if ( net_ses.second == SEND)
                {
                    pkt = net_ses.first->GetPacketToSend();
                    // Elabora pacchetto
                    // Invia nel socket
                    delete pkt;
                }
                else // Recive
                {            
                    // Prendi pacchetto dal socket
                    // Elabora pacchetto 
                    net_ses.first->QueuePacket(pkt);
                }               
            }
            return 0;
        }
};

NetworkManager::NetworkManager() : sem(m_mutex)
{

}

int NetworkManager::Initialize(uint32 n_thread)
{     
    m_thread = n_thread;
    // +1 per l'epoll thread
    net_engine.Initialize(m_thread + 1);
    if (ActivateEpoll() != 0)
        return -1;
    if (ActivateThreadsNetwork() != 0)
        return -1;
    return 0;
}

int NetworkManager::ActivateEpoll()
{  
    if (net_engine.Execute(new SocketThread(*this, 0)) != 0)
    {
        // TODO Log Errore
        return -1;
    }

    return 0;
}

int NetworkManager::ActivateThreadsNetwork()
{  
    for (uint8 i = 0; i < m_thread; i++)
    {
        if (net_engine.Execute(new NetworkThread(*this, 0)) != 0)
        {
            // TODO Log Errore
            return -1;
        }
    }

    return 0;
}

int NetworkManager::QueueSend(Session_smart m_ses)
{   
    if (!m_ses.get())
        return -1;

    q_request.add(netsession_pair(m_ses, SEND));
    sem.Signal();
    return 0;
}

int NetworkManager::QueueRecive(Session_smart m_ses)
{   
    if (!m_ses.get())
        return -1;

    q_request.add(netsession_pair(m_ses, RECIVE));
    sem.Signal();
    return 0;
}

netsession_pair NetworkManager::GetNextSession()
{
    sem.Wait();
    netsession_pair ret;
    q_request.next(ret);
    return ret;
}
