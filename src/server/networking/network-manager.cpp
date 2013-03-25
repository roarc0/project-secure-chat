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

            while (active)
            {
                netsession_pair net_ses = m_netmanager.GetNextSession();
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
                            INFO("debug", "NETWORKTHREAD: WARNING Packet to send NULL\n");
                    }
                    else // Recv
                    {
                        INFO("debug", "NETWORKTHREAD: Recv Packet Event\n");
                        pkt = net_ses.first->RecvPacketFromSocket();
                        if (pkt)                  
                            net_ses.first->QueuePacket(pkt);
                        else
                            INFO("debug", "NETWORKTHREAD: WARNING Packet Recv NULL\n");
                    }
                }
                catch(SocketException e)
                {
                    INFO("debug", "NETWORKTHREAD: %s , closing socket\n", e.what());
                    net_ses.first->m_Socket->CloseSocket();
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
    if (net_engine.Execute(new SocketServer(*this, 0)) != 0)
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

int NetworkManager::QueueRecive(uint32 sock)
{
    Session_smart m_ses = s_manager->FindSession(sock);
    if (!m_ses.get())
        return -1;

    q_request.add(netsession_pair(m_ses, RECV));
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
