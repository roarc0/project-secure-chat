#include "sessionbase.h"

SessionBase::SessionBase()
{
    m_Socket = NULL;
    s_enc = enc_AES256;
    //s_key.clear();
    s_key << "11111222223333344444555556666677";
}

SessionBase::SessionBase(int pSock)
{
    m_Socket = new SocketBase(pSock);
    s_enc = enc_AES256;
    //s_key.clear();
    s_key << "11111222223333344444555556666677";
}

SessionBase::~SessionBase()
{
    INFO("debug", "SESSIONBASE: distruttore\n");

    Packet* packet = NULL;
    while (_recvQueue.next(packet))
        delete packet;

    while (_sendQueue.next(packet))
        delete packet;

    if (m_Socket)
        delete m_Socket;
}

void SessionBase::QueuePacket(Packet* new_packet)
{
    _recvQueue.add(new_packet);
}

Packet* SessionBase::GetPacketToSend()
{
    Packet* pkt = NULL;
    _sendQueue.next(pkt); // TODO usare i ritorni a funzione
    return pkt;
}

void SessionBase::SendPacket(Packet* new_packet)
{
    if (!m_Socket || m_Socket->IsClosed() || !new_packet)
        return;

    if (_SendPacket(*new_packet) == -1)
        m_Socket->CloseSocket();
}

void SessionBase::SendPacketToSocket(Packet* new_packet)
{
    if (!m_Socket || m_Socket->IsClosed() || !new_packet)
        return;

    if (_SendPacketToSocket(*new_packet) == -1)
        m_Socket->CloseSocket();
}

int SessionBase::_SendPacket(Packet& pct)
{
    Packet* pkt = new Packet(pct);
    _sendQueue.add(pkt);
    return 0;
}

int SessionBase::_SendPacketToSocket(Packet& pct)
{
    INFO("debug", "SESSIONBASE: Sending Packet: \"%s\"\n", pct.contents());
    unsigned char* rawData; 

    if (IsEncrypted() && pct.size())
    {
        INFO("debug", "SESSIONBASE: Encrypting Packet  <%d bytes>",pct.size());
        pct.Encrypt(s_key);
    }

    PktHeader header(pct.size()/*+OPCODE_SIZE*/, pct.GetOpcode());
    
    rawData = new unsigned char[header.getHeaderLength() + pct.size() + 1]; // il + 1 non ci serve ?!
    memcpy((void*)rawData, (char*) header.header, header.getHeaderLength());
    memcpy((void*)(rawData + header.getHeaderLength()), (char*) pct.contents(), pct.size());
    
    m_Socket->Send(rawData, pct.size() + header.getHeaderLength());
    delete[] rawData;
    INFO("debug", "SESSIONBASE: Packet <%d bytes> Sent: \"%s\"\n", pct.size(), pct.contents());
    return 0;
}

Packet* SessionBase::RecvPacketFromSocket()
{
    if (!m_Socket || m_Socket->IsClosed())
        return NULL;
    INFO("debug","SESSIONBASE: calling recv\n");
    return _RecvPacketFromSocket();
}

Packet* SessionBase::_RecvPacketFromSocket()
{
    char header[HEADER_SIZE];
    m_Socket->Recv((void*) &header, HEADER_SIZE);
    PktHeader pkt_head(header, HEADER_SIZE);

    if (pkt_head.getSize())
    {
        buffer = new char[pkt_head.getSize()];
        m_Socket->Recv((void*) buffer, pkt_head.getSize());
    }

    INFO("debug","SESSIONBASE: msg: %s , header %u, len %u\n", buffer, pkt_head.getHeader(), pkt_head.getSize());

    pct = new Packet(pkt_head.getHeader(), pkt_head.getSize());
    
    if(!pct)
        return NULL;
    
    if (pkt_head.getSize())
    {
        pct->append((char*)buffer, pkt_head.getSize());

        if (IsEncrypted() && pkt_head.getSize())
        {
            INFO("debug", "SESSIONBASE: Decrypting Packet\n");
            pct->Decrypt(s_key);
            INFO("debug", "SESSIONBASE: Packet Decrypted\n");
        }

        delete[] buffer;
    }
    return pct;
}

void SessionBase::Handle_NULL(Packet& /*packet*/)
{
    // LOG
};
