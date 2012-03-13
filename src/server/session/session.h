#include "../../shared/session/sessionbase.h"

class Session : public SessionBase
{
    public:
        Session(Socket* pSock);
        ~Session();

        bool Update(uint32 diff, PacketFilter& updater);

        // THREADUNSAFE
        void KickSession();
        void SetId(uint32 id) { m_id = id; }

        // THREADSAFE 
        bool IsInChannel() { return channel_name == "" ? false : true; }
        
        uint32 GetId() { return m_id; }

        void SendWaitQueue(int position);
  
    private:
        uint32 m_id;

        bool m_inQueue;
        // Channel
        std::string channel_name;
};
