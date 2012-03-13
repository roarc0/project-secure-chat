#include "session.h"

SessionBase::SessionBase(Socket* pSock)
{
    m_Socket = pSock;
}

SessionBase::~SessionBase()
{
    // empty incoming packet queue
    Packet* packet = NULL;
    while (_recvQueue.next(packet))
        delete packet;
}

void SessionBase::QueuePacket(Packet* new_packet)
{
    _recvQueue.add(new_packet);
}

void SessionBase::SendPacket(Packet* new_packet)
{
    if (!m_Socket)
        return;

    if (m_Socket->SendPacket(*new_packet) == -1)
        m_Socket->CloseSocket();
}
