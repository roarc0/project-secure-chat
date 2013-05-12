#ifndef PACKET_FILTER_H
#define PACKET_FILTER_H

#include "networking/packet.h"
#include "session/sessionbase.h"

class PacketFilter
{
public:
    explicit PacketFilter(SessionBase* pSession) : m_pSession(pSession) {}
    virtual ~PacketFilter() {}

    virtual bool IsSingleSessionFilter() { return false; } 
    virtual bool Process(Packet* /*packet*/) { return true; }
    virtual bool ProcessLogout() const { return true; }

protected:
    SessionBase* const m_pSession;
};

//process only thread-safe packets
class MapSessionFilter : public PacketFilter
{
public:
    explicit MapSessionFilter(SessionBase* pSession) : PacketFilter(pSession) {}
    ~MapSessionFilter() {}

    virtual bool Process(Packet* packet);
    // Il logout non può essere eseguito dal MultiThread
    virtual bool ProcessLogout() const { return false; }
};

//class used to filer only thread-unsafe packets from queue
class SingleSessionFilter : public PacketFilter
{
public:
    explicit SingleSessionFilter(SessionBase* pSession) : PacketFilter(pSession) {}
    ~SingleSessionFilter() {}

    virtual bool IsSingleSessionFilter() { return true; } 
    virtual bool Process(Packet* packet);
};

#endif
