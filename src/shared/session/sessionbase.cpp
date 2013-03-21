#include "sessionbase.h"

SessionBase::SessionBase(SocketBase* pSock)
{
    m_Socket = pSock;
}

SessionBase::~SessionBase()
{
    // empty incoming packet queue
    Packet* packet = NULL;
    while (_recvQueue.next(packet))
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
    PktHeader header(pct.size()+OPCODE_SIZE, pct.GetOpcode());

    unsigned char* rawData = new unsigned char[header.getHeaderLength()+ pct.size() + 1];
    
    // Inserire Criptazione

    memcpy((void*)rawData, (char*) header.header, header.getHeaderLength());
    memcpy((void*)rawData + header.getHeaderLength(), (char*) pct.contents(), pct.size());

    m_Socket->Send(rawData, pct.size() + header.getHeaderLength());
    delete[] rawData;
}

void SessionBase::Handle_NULL(Packet& /*packet*/) 
{
    // LOG
};
