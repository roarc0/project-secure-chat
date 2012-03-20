#include "network-manager.h"

class SocketThread: public MethodRequest
{
    private:

        NetworkManager& m_netmanager;
        uint32 m_diff;

    public:

        SocketThread(NetworkManager& netmanager, uint32 d) : 
        MethodRequest(), m_netmanager(netmanager), m_diff(d)
        {
            
        }

        ~SocketThread()
        {

        }

        int Call()
        {
            //inizializzazione callback
            while (1)
            {
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

    public:

        NetworkThread(NetworkManager& netmanager, uint32 d) : 
        MethodRequest(), m_netmanager(netmanager), m_diff(d)
        {
            
        }

        ~NetworkThread()
        {

        }

        int Call()
        {
            Session* ses = NULL;
            while (1)
            {                
                ses = m_netmanager.GetNextSession();
                // prendi pacchetto dal socket
                // elabora pacchetto 
                // Inserisci in coda pacchetto nella session                
            }
            return 0;
        }
};

NetworkManager::NetworkManager() : sem(m_mutex)
{

}

int NetworkManager::Queue(Session* m_ses)
{   
    if (!m_ses)
        return -1;

    q_request.add(m_ses);
    sem.Signal();
    return 0;
}

Session* NetworkManager::GetNextSession()
{
    sem.Wait();
    Session* ret = NULL;
    q_request.next(ret);
    return ret;
}
