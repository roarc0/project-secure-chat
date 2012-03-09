#include "../../shared/networking/packetfilter.h"
#include "../../shared/networking/packet.h"
#include "../../shared/utility/lockedqueue.h"
include "../../shared/threading/lock.h"

class Session
{
    public:
        Session(Socket* pSock);
        ~Session();

        bool Update(uint32 diff, PacketFilter& updater);

        // THREADUNSAFE
        void KickSession();
        void SetId(uint32 id) { m_id = id; }

        // THREADSAFE
        void QueuePacket(Packet* new_packet); 
        void SendPacket(Packet* new_packet);  
        
        uint32 GetId() { return m_id; }

        void SendWaitQueue(int position);
  
    private:
        Mutex m_mutex;

        uint32 m_id;

        LockedQueue<Packet*> _recvQueue;
        // If Session is in Qeuue
        bool m_inQueue;
        // Socket
        Socket* m_Socket;
};
