#include "network-manager.h"
#include "session-manager.h"

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
            Packet* pkt = NULL;
            
            INFO("debug", "NETWORKTHREAD: Started\n");
            
            while (active)
            {
                INFO("debug", "NETWORKTHREAD: Wait\n");
                netsession_pair net_ses = m_netmanager.GetNextSession();
                INFO("debug", "NETWORKTHREAD: Got Signal!\n");
                
                try
                {
                    if (net_ses.second == SEND)
                    {                        
                        pkt = net_ses.first->GetPacketToSend();
                        if (pkt)
                        {
                            INFO("debug", "NETWORKTHREAD: Send Packet Event\n");
                            net_ses.first->SendPacketToSocket(pkt);
                            delete pkt;
                        }
                        else
                            INFO("debug", "NETWORKTHREAD: WARNING packet to send is NULL\n");
                    }
                    else // Recv
                    {
                        INFO("debug", "NETWORKTHREAD: Recv Packet Event\n");
                        pkt = net_ses.first->RecvPacketFromSocket();
                        if (pkt)
                        {   
                            INFO("debug", "NETWORKTHREAD: Accodato pacchetto in coda elaborazione\n");          
                            net_ses.first->QueuePacket(pkt);
                        }
                        else
                            INFO("debug", "NETWORKTHREAD: WARNING packet received is NULL\n");
                    }
                }
                catch(SocketException e)
                {
                    INFO("debug", "NETWORKTHREAD: %s , closing socket\n", e.what());
                    net_ses.first->m_Socket->CloseSocket();
                }
            }
            
            INFO("debug", "NETWORKTHREAD: Killed\n");
            
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
    INFO("debug","NETWORKMANAGER: Starting %d threads\n", (m_thread + 1));
    net_engine.Initialize(m_thread + 1);
    if (ActivateThreadsNetwork() != 0)
        return -1;
    if (ActivateEpoll() != 0)
        return -1;
    return 0;
}

int NetworkManager::ActivateEpoll()
{
    if (net_engine.Execute(new SocketServer(*this, 0)) != 0)
    {
        INFO ("debug", "NETWORKMANAGER: fail activate epoll \n");
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
            INFO ("debug", "NETWORKMANAGER: fail activate network thread \n");
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
    
    INFO("debug","NETWORK-MANAGER: queue send\n");
   
    return 0;
}

int NetworkManager::QueueRecive(uint32 sock)
{
    Session_smart m_ses = s_manager->FindSession(sock);
    if (!m_ses.get())
    {
        INFO("debug","NETWORK-MANAGER: session for socket %d not found\n", sock);
        return -1;
    }

    q_request.add(netsession_pair(m_ses, RECV));
    sem.Signal();
    
    INFO("debug","NETWORK-MANAGER: queue recive\n");
    
    return 0;
}

netsession_pair NetworkManager::GetNextSession()
{
    sem.Wait();
    netsession_pair ret;
    q_request.next(ret);
    return ret;
}
