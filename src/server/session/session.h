#include "../../shared/session/user-session.h"
#include "../../shared/threading/mutex.h"
#include "../../shared/networking/packetfilter.h"
#include "../../shared/networking/packet.h"
#include "../../shared/utility/lockedqueue.h"

class Session
{
    public:
        Session(UserSession* pUser);
        ~Session();

        bool Update(PacketFilter& updater);

        // TODO Inserire tempo di creazione della session per controllo di pacchetti precedenti
        void SetSession(UserSession* pUser, uint32 id) { m_pUser = pUser; m_pUser->SetId(id); m_pUser->SetSession(this);}
        UserSession* GetUserSession() { return m_pUser; }

        bool IsInCell() { return m_incell; }
        void SetInCell(bool b_incell) { m_incell = b_incell; }

        void SetInQueue(bool state) { m_inQueue = state; }
        void QueuePacket(Packet* new_packet);        
  
    private:

        UserSession* m_pUser;
            
        LockedQueue<Packet*> _recvQueue;
        // If we are in a Cell
        bool m_incell;
        // If Session is in Qeuue
        bool m_inQueue;
        // Socket
        int m_Socket;
};
