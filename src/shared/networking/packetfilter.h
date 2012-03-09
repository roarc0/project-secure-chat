#ifndef PACKET_FILTER_H
#define PACKET_FILTER_H

class PacketFilter
{
public:
    explicit PacketFilter(Session* pSession) : m_pSession(pSession) {}
    virtual ~PacketFilter() {}

    virtual bool Process(Packet* /*packet*/) { return true; }
    virtual bool ProcessLogout() const { return true; }

protected:
    Session* const m_pSession;
};

//process only thread-safe packets
class CellSessionFilter : public PacketFilter
{
public:
    explicit CellSessionFilter(Session* pSession) : PacketFilter(pSession) {}
    ~CellSessionFilter() {}

    virtual bool Process(Packet* packet);
    // Il logout non può essere eseguito dal MultiThread
    virtual bool ProcessLogout() const { return false; }
};

//class used to filer only thread-unsafe packets from queue
class SingleSessionFilter : public PacketFilter
{
public:
    explicit SingleSessionFilter(Session* pSession) : PacketFilter(pSession) {}
    ~SingleSessionFilter() {}

    virtual bool Process(Packet* packet);
};

#endif
