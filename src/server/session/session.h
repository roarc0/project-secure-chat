#ifndef _SESSION_H
#define _SESSION_H

#include "session/sessionbase.h"
#include "utility/counted_ptr.h"
#include "packetfilter.h"
#include "channel.h"

#if COMPILER == COMPILER_GNU
#  define ATTR_PRINTF(F, V) __attribute__ ((format (printf, F, V)))
#else //COMPILER != COMPILER_GNU
#  define ATTR_PRINTF(F, V)
#endif //COMPILER == COMPILER_GNU

#define MIN_REFRESH_KEY_INTERVAL 60

class Session;
class Channel;
typedef counted_ptr<Session> Session_smart;
typedef counted_ptr<Channel> SmartChannel;

class Session : public SessionBase
{
    public:
        Session(int pSock);
        ~Session();

        bool Update(uint32 diff, PacketFilter& updater);
        virtual bool IsServer() { return true; }

        // THREADUNSAFE        
        void SetId(uint32 id) { m_id = id; }
        void setSmartPointer(Session_smart m_ses);
        void deleteSmartPointer();
        void setChannel(SmartChannel pChan) { m_channel=(SmartChannel)pChan; }
        SmartChannel getChannel() { return m_channel; }
        void GenerateNewKey(uint32 diff);

        // THREADSAFE 
        bool IsInChannel() { return m_channel.get() ? true : false; }        
        uint32 GetId() { return m_id; }
        void SendWaitQueue(int position);
        void SetInQueue(bool state) { m_inQueue = state; }    

        // Handle
        void HandlePing(Packet& packet); 
        void HandleServerSide(Packet& packet);
        void HandleMessage(Packet& packet); 
        void HandleWhisp(Packet& packet);
        void HandleJoinChannel(Packet& packet); 
        void HandleCreateChannel(Packet& packet); 
        void HandleLeaveChannel(Packet& packet); 
        void HandleListChannel(Packet& packet);
        void HandleLogin(Packet& packet);
        void HandleRefreshKey(Packet&);
        
        void SendSysMessage(const char *str);
        void PSendSysMessage(const char *format, ...) ATTR_PRINTF(2, 3);
        void InitKeyUpdateInterval();
 
    private:

        int _SendPacket(Packet& pct);
        int _SendPacket(Packet* pct);
    
        uint32 m_id;
        Session_smart smartThis;
        bool m_inQueue;
        SmartChannel m_channel;
        IntervalTimer i_timer_key; 
};

#endif
