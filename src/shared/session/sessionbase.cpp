#include "sessionbase.h"

SessionBase::SessionBase()
{
    m_Socket = NULL;
    s_status = STATUS_DISCONNECTED;
    s_enc = ENC_NONE;
}

SessionBase::SessionBase(int pSock)
{
    m_Socket = new SocketBase(pSock);
    s_status = STATUS_CONNECTED;  
    s_enc = ENC_NONE;
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

void SessionBase::SendPacketToSocket(Packet* new_packet, unsigned char* temp_buffer)
{
    if (!m_Socket || m_Socket->IsClosed() || !new_packet)
        return;

    if (_SendPacketToSocket(*new_packet, temp_buffer) == -1)
        m_Socket->CloseSocket();
}

int SessionBase::_SendPacket(Packet& pct)
{
    INFO("debug", "SESSION_BASE: _SendPacket\n");
    Packet* pkt = new Packet(pct);
    _sendQueue.add(pkt);
    return 0;
}

int SessionBase::_SendPacketToSocket(Packet& pkt, unsigned char* temp_buffer)
{
    INFO("debug", "SESSION_BASE: sending packet: \"%s\"\n", pkt.contents());
    unsigned char* rawData;

    Packet pct(0); // TODO inserire header appositi
    pct.Incapsulate(pkt);

    if (IsEncrypted() && pct.size())
    {
        INFO("debug", "SESSION_BASE: encrypting packet  <%d bytes>\n", pct.size());
        pct.Encrypt(s_key);
    }
   
    PktHeader header(pct.GetOpcode(), pct.size());
    
    if (!temp_buffer)
        rawData = new unsigned char[header.getHeaderLength() + pct.size() + 1]; // il + 1 non ci serve ?!
    else 
        rawData = temp_buffer;

    memcpy((void*)rawData, (char*) header.header, header.getHeaderLength());
    memcpy((void*)(rawData + header.getHeaderLength()), (char*) pct.contents(), pct.size());

    m_Socket->Send(rawData, pct.size() + header.getHeaderLength());

    if (!temp_buffer)
        delete[] rawData;

    INFO("debug", "SESSION_BASE: packet <%d bytes> sent\n", pct.size());
    return 0;
}

Packet* SessionBase::RecvPacketFromSocket(unsigned char* temp_buffer)
{
    if (!m_Socket || m_Socket->IsClosed())
        return NULL;
    INFO("debug","SESSION_BASE: receiving packet\n");
    return _RecvPacketFromSocket(temp_buffer);
}

Packet* SessionBase::_RecvPacketFromSocket(unsigned char* temp_buffer)
{
    char header[HEADER_SIZE];
    m_Socket->Recv((void*) &header, HEADER_SIZE);
    PktHeader pkt_head((char*)header, HEADER_SIZE);

    unsigned char* buffer;

    if (pkt_head.getSize())
    {
        if (!temp_buffer)
            buffer = new unsigned char[pkt_head.getSize()];
        else
            buffer = temp_buffer;
        m_Socket->Recv((void*) buffer, pkt_head.getSize());
    }

    INFO("debug","SESSION_BASE: packet [header %d, length %d]\n", pkt_head.getHeader(), pkt_head.getSize());

    pct = new Packet(pkt_head.getHeader(), pkt_head.getSize());
    
    if (!pct)
        return NULL;
    
    Packet* pkt = NULL;

    if (pkt_head.getSize())
    {
        pct->append((char*)buffer, pkt_head.getSize());

        if (IsEncrypted() && pkt_head.getSize())
        {
            //INFO("debug", "SESSION_BASE: decrypting packet\n");
            pct->Decrypt(s_key);
            //INFO("debug", "SESSION_BASE: packet decrypted\n");
        }

        INFO("debug","SESSION_BASE: packet content:\n");
        pct->hexlike();

        pkt = pct->Decapsulate();

        delete pct;

        if (!temp_buffer)
            delete[] buffer;
    }
    else
    {
        INFO("debug","SESSION_BASE: BAD BAD BAD! Must not exit! \n");
        assert(false);
    }

    return pkt;
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

bool SessionBase::IsRejected() const
{
    return s_status == STATUS_REJECTED;
    
}

void SessionBase::SetConnected(bool c)
{
    if(c)
        s_status = STATUS_CONNECTED;
    else
    {
        s_status = STATUS_DISCONNECTED;
        s_enc = ENC_NONE;
        m_Socket->CloseSocket();
    }
}

SessionStatus SessionBase::GetSessionStatus() const
{
    return s_status;
}

void SessionBase::SetSessionStatus(SessionStatus s)
{
    s_status = s;
}
