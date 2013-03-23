#include "sessionbase.h"

SessionBase::SessionBase()
{
    m_Socket = NULL;
}

SessionBase::SessionBase(int pSock)
{
    m_Socket = new SocketBase(pSock);
}

SessionBase::~SessionBase()
{
    INFO("debug", "DISTRUTTORE!\n");

    Packet* packet = NULL;
    while (_recvQueue.next(packet))
        delete packet;

    while (_sendQueue.next(packet))
        delete packet;    

    if(m_Socket)
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

void SessionBase::SendPacketToSocket(Packet* new_packet)
{
    if (!m_Socket || !new_packet)
        return;

    if (_SendPacketToSocket(*new_packet) == -1)
        m_Socket->CloseSocket();
}

int SessionBase::_SendPacket(const Packet& pct)
{
    Packet* pkt = new Packet(pct);
    _sendQueue.add(pkt);
    return 0;
}

int SessionBase::_SendPacketToSocket(const Packet& pct)
{
    PktHeader header(pct.size()/*+OPCODE_SIZE*/, pct.GetOpcode());

    unsigned char* rawData = new unsigned char[header.getHeaderLength()+ pct.size() + 1];
    
    // Inserire Criptazione

    memcpy((void*)rawData, (char*) header.header, header.getHeaderLength());
    memcpy((void*)rawData + header.getHeaderLength(), (char*) pct.contents(), pct.size());

    m_Socket->Send(rawData, pct.size() + header.getHeaderLength());
    delete[] rawData;

    return 0;
}

Packet* SessionBase::RecivePacketFromSocket()
{
    if (!m_Socket)
        return NULL;

    return _RecivePacketFromSocket();
}

Packet* SessionBase::_RecivePacketFromSocket()
{
    char header[4];
    // Prendi Header
    m_Socket->Recv((void*) &header, 4);
    PktHeader pkt_head(header, 4);
    // Prendi Resto dei Dati
    
    char buf[pkt_head.getSize()+1];
    m_Socket->Recv((void*) &buf, pkt_head.getSize());  
                        
    INFO("debug","Livello Network Messaggio: %s , header %u, lunghezza %u\n", buf, pkt_head.getHeader(), pkt_head.getSize()); 

    // Impacchetta                      
    Packet* pkt = new Packet(pkt_head.getHeader(), pkt_head.getSize());
    *pkt << buf;

    return pkt;
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
