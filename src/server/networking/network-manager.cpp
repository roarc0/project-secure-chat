#include "network-manager.h"
#include "session-manager.h"

class NetworkThread: public MethodRequest
{
    private:

        NetworkManager& m_netmanager;
        uint32 m_diff;
        bool active;
        unsigned char* buffer;

    public:

        NetworkThread(NetworkManager& netmanager, uint32 d) :
        MethodRequest(), m_netmanager(netmanager), m_diff(d), active(true)
        {
            buffer = new unsigned char[(uint32(1)<<16)-1]; // max of uint16
        }

        ~NetworkThread()
        {
            delete[] buffer;
        }

        int Call()
        {
            Packet* pkt = NULL;
            
            INFO("debug", "NETWORK_THREAD: started\n");
            
            while (active)
            {
                INFO("debug", "NETWORK_THREAD: wait\n");
                netsession_pair net_ses = m_netmanager.GetNextSession();
                INFO("debug", "NETWORK_THREAD: got signal!\n");
                
                try
                {
                    if (net_ses.second == SEND)
                    {                        
                        pkt = net_ses.first->GetPacketToSend();
                        if (pkt)
                        {
                            INFO("debug", "NETWORK_THREAD: send packet event\n");
                            net_ses.first->SendPacketToSocket(pkt, buffer);
                            delete pkt;
                        }
                        else
                            INFO("debug", "NETWORK_THREAD: WARNING can't send NULL packet\n");
                    }
                    else
                    {
                        INFO("debug", "NETWORK_THREAD: receiving packet event\n");
                        pkt = net_ses.first->RecvPacketFromSocket(buffer);
                        if (pkt)
                        {
                            INFO("debug", "NETWORK_THREAD: packet queued in elaboration queue.\n");          
                            net_ses.first->QueuePacket(pkt);    
                        }
                    }
                }
                catch(SocketException e)
                {
                    INFO("debug", "NETWORK_THREAD: %s , closing socket\n", e.what());
                    net_ses.first->Close();
                }
            }
            
            INFO("debug", "NETWORK_THREAD: killed\n");
            
            return 0;
        }
};

NetworkManager::NetworkManager() : sem(m_mutex)
{

}

int NetworkManager::Initialize(uint32 n_thread)
{
    m_thread = n_thread;

    INFO("debug","NETWORK_MANAGER: starting %d threads\n", (m_thread + 1));
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
        INFO ("debug", "NETWORK_MANAGER: can't start epoll thread\n");
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
            INFO ("debug", "NETWORK_MANAGER: can't start network thread\n");
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
    
    INFO("debug","NETWORK_MANAGER: send queued\n");
   
    return 0;
}

int NetworkManager::QueueRecive(uint32 sock)
{
    Session_smart m_ses = s_manager->FindSession(sock);
    if (!m_ses.get())
    {
        INFO("debug","NETWORK_MANAGER: session for socket %d not found\n", sock);
        return -1;
    }

    q_request.add(netsession_pair(m_ses, RECV));
    sem.Signal();
    
    INFO("debug","NETWORK_MANAGER: recive queued\n");
    
    return 0;
}

netsession_pair NetworkManager::GetNextSession()
{
    sem.Wait();
    netsession_pair ret;
    q_request.next(ret);
    return ret;
}
