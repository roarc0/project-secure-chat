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
    INFO("debug", "SESSIONBASE: Inizio invio pacchetto nel Socket %s\n", pct.contents());
    PktHeader header(pct.size()/*+OPCODE_SIZE*/, pct.GetOpcode());
    unsigned char* rawData = new unsigned char[header.getHeaderLength() + pct.size() + 1];
    
    // Inserire Criptazione
    if (IsEncrypted())
    {
        INFO("debug", "SESSIONBASE: Encrypting Packet");
        pct.Encrypt(s_key);
    }
    
    memcpy((void*)rawData, (char*) header.header, header.getHeaderLength());
    memcpy((void*)(rawData + header.getHeaderLength()), (char*) pct.contents(), pct.size());
    
    m_Socket->Send(rawData, pct.size() + header.getHeaderLength());
    delete[] rawData;
    INFO("debug", "SESSIONBASE: Pacchetto Inviato nel Socket %s\n", pct.contents());
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
    char header[4];
    // Prendi Header
    m_Socket->Recv((void*) &header, 4);
    PktHeader pkt_head(header, 4);

    // Prendi Resto dei Dati
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

        if (IsEncrypted())
        {
            INFO("debug", "SESSIONBASE: Decrypting Packet");
            pct->Decrypt(s_key);
        }

        delete[] buffer;
    }
    return pct;
}

void SessionBase::Handle_NULL(Packet& /*packet*/)
{
    // LOG
};
