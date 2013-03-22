#include "sessionbase.h"

SessionBase::SessionBase(int pSock)
{
    m_Socket = new SocketBase(pSock);
}

SessionBase::~SessionBase()
{
    INFO("debug", "DISTRUTTORE!\n");
    // empty incoming packet queue
    Packet* packet = NULL;
    while (_recvQueue.next(packet))
        delete packet;

    while (_sendQueue.next(packet))
        delete packet;    

    delete m_Socket;
}

void SessionBase::QueuePacket(Packet* new_packet)
{
    _recvQueue.add(new_packet);
}

void SessionBase::SendPacket(Packet* new_packet)
{
    if (!m_Socket || !new_packet)
        return;

    if (_SendPacket(*new_packet) == -1)
        m_Socket->CloseSocket();
}

int SessionBase::_SendPacket(const Packet& pct)
{
    // Packet pkt = new Packet(pct);
    //_sendQueue.add(pkt);
    // return 0;

    PktHeader header(pct.size()/*+OPCODE_SIZE*/, pct.GetOpcode());

    unsigned char* rawData = new unsigned char[header.getHeaderLength()+ pct.size() + 1];
    
    // Inserire Criptazione

    memcpy((void*)rawData, (char*) header.header, header.getHeaderLength());
    memcpy((void*)rawData + header.getHeaderLength(), (char*) pct.contents(), pct.size());

    m_Socket->Send(rawData, pct.size() + header.getHeaderLength());
    delete[] rawData;
}

Packet* SessionBase::GetPacketToSend()
{
     Packet* pkt;
     _recvQueue.next(pkt);
    return pkt;
}

void SessionBase::Handle_NULL(Packet& /*packet*/) 
{
    // LOG
};
