#include "../../shared/networking/packetfilter.h"
#include "../../shared/networking/packet.h"
#include "../../shared/utility/lockedqueue.h"

class Session
{
    public:
        Session(Socket* pSock);
        ~Session();

        bool Update(int diff, PacketFilter& updater);

        bool IsInCell() { return m_incell; }
        void SetInCell(bool state) { m_incell = state; }
        void IsInQueue() { return m_inQueue; }
        void SetInQueue(bool state) { m_inQueue = state; }

        void KickSession();

        void QueuePacket(Packet* new_packet);  

        void SetId(uint32 id) { m_id = id; }
        uint32 GetId() { return m_id; }
  
    private:
        uint32 m_id;

        LockedQueue<Packet*> _recvQueue;
        // If we are in a Cell
        bool m_incell;
        // If Session is in Qeuue
        bool m_inQueue;
        // Socket
        int m_Socket;
};
