#include "sessionbase.h"

SessionBase::SessionBase()
{
    m_Socket = NULL;
    s_status = STATUS_DISCONNECTED;

    // TODO REMOVE
    s_enc = ENC_AES256; // ENC_NONE;
    s_key << "11111222223333344444555556666677";
}

SessionBase::SessionBase(int pSock)
{
    m_Socket = new SocketBase(pSock);
    s_status = STATUS_CONNECTED;
    
    // TODO REMOVE
    s_enc = ENC_AES256; // ENC_NONE;
    s_key << "11111222223333344444555556666677";
}

SessionBase::~SessionBase()
{
    INFO("debug", "SESSION_BASE: destructor\n");

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
    
    INFO("debug", "SESSION_BASE: SendPacket\n");

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
    INFO("debug", "SESSION_BASE: _SendPacket\n");
    Packet* pkt = new Packet(pct);
    _sendQueue.add(pkt);
    return 0;
}

int SessionBase::_SendPacketToSocket(Packet& pct)
{
    INFO("debug", "SESSION_BASE: sending packet: \"%s\"\n", pct.contents());
    unsigned char* rawData;

    if (IsEncrypted() && pct.size())
    {
        INFO("debug", "SESSION_BASE: encrypting packet  <%d bytes>\n",pct.size());
        pct.Encrypt(s_key);
    }

    PktHeader header(pct.size()/*+OPCODE_SIZE*/, pct.GetOpcode());
    
    rawData = new unsigned char[header.getHeaderLength() + pct.size() + 1]; // il + 1 non ci serve ?!
    memcpy((void*)rawData, (char*) header.header, header.getHeaderLength());
    memcpy((void*)(rawData + header.getHeaderLength()), (char*) pct.contents(), pct.size());
    
    m_Socket->Send(rawData, pct.size() + header.getHeaderLength());
    delete[] rawData;
    INFO("debug", "SESSION_BASE: packet <%d bytes> sent: \"%s\"\n", pct.size(), pct.contents());
    return 0;
}

Packet* SessionBase::RecvPacketFromSocket()
{
    if (!m_Socket || m_Socket->IsClosed())
        return NULL;
    INFO("debug","SESSION_BASE: receiving packet\n");
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

    INFO("debug","SESSION_BASE: packet [header %d, length %d]\n", pkt_head.getHeader(), pkt_head.getSize());

    pct = new Packet(pkt_head.getHeader(), pkt_head.getSize());
    
    if (!pct)
        return NULL;
    
    if (pkt_head.getSize())
    {
        pct->append((char*)buffer, pkt_head.getSize());

        if (IsEncrypted() && pkt_head.getSize())
        {
            INFO("debug", "SESSION_BASE: decrypting packet\n");
            pct->Decrypt(s_key);
            INFO("debug", "SESSION_BASE: packet decrypted\n");
        }
        
        INFO("debug","SESSION_BASE: packet content:\n");
        pct->hexlike();

        delete[] buffer;
    }
    return pct;
}

void SessionBase::HandleNULL(Packet& /*packet*/)
{
    INFO("debug", "SESSION_BASE: handling null message\n");
}

const std::string* SessionBase::GetUsername()
{
    return &username;
}

bool SessionBase::SetUsername(const std::string& n)
{
    /* TODO validate username ( can't contain white spaces )
    if(username is not valid )
        return false;
    */    
    
    username = n; 
    return true;
}

bool SessionBase::IsEncrypted() const
{
    return s_enc != ENC_NONE;
}

void SessionBase::SetEncryption(const ByteBuffer& key,
                                SessionEncryption type=ENC_AES128)
{
    s_key = key;
    s_enc = type;
}

bool SessionBase::IsAuthenticated() const
{
    return s_status == STATUS_AUTHENTICATED;
}

bool SessionBase::IsConnected() const
{
    return s_status >= STATUS_CONNECTED;
}

void SessionBase::SetConnected(bool c) // reset encryption ecc...
{
    if(c)
        s_status = STATUS_CONNECTED;
    else
        s_status = STATUS_DISCONNECTED;
}

SessionStatus SessionBase::GetSessionStatus() const
{
    return s_status;
}
