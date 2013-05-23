#include "sessionbase.h"
#include "networking/opcode.h"

SessionBase::SessionBase()
{
    m_Socket = NULL;
    s_status = STATUS_DISCONNECTED;
    s_enc = ENC_NONE;
    s_next_enc = ENC_UNSPEC;
    u_changekeys = 0;

    u_id_send = 0;
    u_id_receive = 0;
}

SessionBase::SessionBase(int pSock)
{
    m_Socket = new SocketBase(pSock);
    s_status = STATUS_CONNECTED;  
    s_enc = ENC_NONE;
    s_next_enc = ENC_UNSPEC;
    u_changekeys = 0;
}

SessionBase::~SessionBase()
{
    INFO("debug", "SESSION_BASE: destructor\n");

    Packet* packet = NULL;
    while (_recvQueue.next(packet))
        if (packet)
            delete packet;

    while (_sendQueue.next(packet))
        if (packet)
            delete packet;

    if (m_Socket)
        delete m_Socket;
}

void SessionBase::Close()
{
    if (m_Socket)
        m_Socket->CloseSocket();
}

void SessionBase::QueuePacket(Packet* new_packet)
{
    _recvQueue.add(new_packet);
}

Packet* SessionBase::GetPacketToSend()
{
    if (!m_Socket || m_Socket->IsClosed())
        return NULL;

    Packet* pkt = NULL;
    _sendQueue.next(pkt);
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

int SessionBase::_SendPacketToSocket(Packet& pkt, unsigned char* temp_buffer)
{
    if (IsServer())
    {
        Lock guard(m_mutex);
    }

    INFO("debug", "SESSION_BASE: sending packet: \"%s\"\n", pkt.contents());
    unsigned char* rawData = NULL;

    try
    {
        Packet pct(0);
        pct.Incapsulate(pkt, u_id_send, IsSymmetric());
               
        if (IsEncrypted() && pct.size())
        {
            ByteBuffer par;
            string str_enc;
            
            switch (s_enc)
            {
                case ENC_AES128:
                case ENC_AES256:
                    str_enc = "AES";
                    par = s_key;
                    pct.SetMode(MODE_AES);
                break;
                case ENC_RSA:
                    str_enc = "RSA";
                    par << f_other_pub_key;
                    pct.SetMode(MODE_RSA);
                break;
                default:
                    
                break; 
            }
            
            INFO("debug", "SESSION_BASE: encrypting %s packet  <%d bytes>\n",
                          str_enc.c_str(), pct.size());
            pct.Encrypt(par);
        }
       
        //PktHeader header(pct.GetOpcode(), pct.size());
        uint16 len = pct.size();
        
        if (!temp_buffer)
            rawData = new unsigned char[2 + pct.size() + 1];
        else 
            rawData = temp_buffer;

        memcpy((void*)rawData, (char*)(&len), 2);
        memcpy((void*)(rawData + 2), (char*) pct.contents(), pct.size());

        m_Socket->Send(rawData, pct.size() + 2);

        if (IsServer() && pkt.GetOpcode() == SMSG_REFRESH_KEY && u_changekeys == 1)
        {
            SetEncryption(s_key_tmp, ENC_AES256);
            u_changekeys = 2;

            // Resettare numerazione pacchetti
            ResetPacketNum();
        }
        else if (IsServer() && s_next_enc == ENC_RSA)
        {
            SetNextEncryption(ENC_UNSPEC);
            SetEncryption(ENC_RSA);
        }

        if (!temp_buffer)
            delete[] rawData;

        INFO("debug", "SESSION_BASE: packet <%d bytes> sent\n", 2+pct.size());
        return 0;

    }
    catch (ByteBufferPositionException e)
    {
        INFO("debug","SESSION_BASE: _SendPacketToSocket: ByteBufferPositionException catched \n");
        if (!temp_buffer && rawData)
            delete[] rawData;
        Close();
        return -1;
    }
}

Packet* SessionBase::RecvPacketFromSocket(unsigned char* temp_buffer)
{
    if (!m_Socket || m_Socket->IsClosed())
        return NULL;

    INFO("debug","SESSION_BASE: receiving packet\n");
    Packet* packet = _RecvPacketFromSocket(temp_buffer);

    if (packet && packet->GetOpcode() >= NUM_MSG_TYPES) // Max opcode
    {
        delete packet; 
        packet = NULL;
        m_Socket->CloseSocket();
        INFO ("debug", "SESSION_BASE: opcode is not valid\n");
    }

    return packet;
}

Packet* SessionBase::_RecvPacketFromSocket(unsigned char* temp_buffer)
{
    if (IsServer())
    {
        Lock guard(m_mutex);
    }
    
    uint16 len;
    unsigned char* buffer = NULL;
    Packet* pct = NULL;

    try 
    {
        m_Socket->Recv((void*) &len, 2);
        //PktHeader pkt_head((char*)header, 2);

        if (len > 65000) // limit 2^16
        {
            INFO("debug","SESSION_BASE: packet > 65000, kicking session\n");
            Close();
            return NULL;
        }    

        INFO("debug","SESSION_BASE: packet [header %d, length %d]\n", 0, len);

        pct = new Packet(0, len);        
        if (!pct)
            return NULL;
        
        Packet* pkt = NULL;

        if ((uint16)len)
        {
            if (!temp_buffer)
            {
                buffer = new unsigned char[len];
                if (!buffer)
                    return NULL;
            }
            else
                buffer = temp_buffer;

            m_Socket->Recv((void*)buffer, len);
            pct->append((char*)buffer, len);

            if (IsEncrypted())
            {
                ByteBuffer par;

                switch (s_enc)
                {
                    case ENC_AES128:
                    case ENC_AES256:
                        par = s_key;
                        pct->SetMode(MODE_AES);
                    break;
                    case ENC_RSA:
                        par << f_my_priv_key;
                        if (HavePassword())
                            par << GetPassword();
                        pct->SetMode(MODE_RSA);
                    break;
                    default:
                    break;                        
                }
            
                pct->Decrypt(par);
            }

            INFO("debug","SESSION_BASE: packet content:\n");
            pct->hexlike();

            pkt = pct->Decapsulate(u_id_receive, IsSymmetric());
            delete pct;
            pct = pkt;

            if (!temp_buffer)
                delete[] buffer;
        }
        else
        {
            INFO("debug","SESSION_BASE: BAD BAD BAD!\n");
            /* kill session ? */
        }

        return pct;
    }
    catch (ByteBufferPositionException e)
    {
        INFO("debug","SESSION_BASE: _RecvPacketFromSocket: ByteBufferPositionException catched \n");
        if (!temp_buffer && buffer)
            delete[] buffer;
        if (pct)
            delete pct;
        Close();
        return NULL;
    }
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
    /*std::string::size_type index = n.find_last_not_of(EXCLUDED_CHARS);
    if (index != std::string::npos)
        return false;*/
      
    if (n.length() > MAX_USER_LEN)
        return false;
    
    username = n;
    
    return true;
}

bool SessionBase::IsEncrypted() const
{
    return s_enc != ENC_NONE;
}

bool SessionBase::IsSymmetric() const
{
    return (s_enc == ENC_AES256 || s_enc == ENC_AES128);
}

void SessionBase::SetEncryption(const ByteBuffer& key,
                                SessionEncryption type=ENC_AES128)
{
    s_key = key;
    s_enc = type;
}

void SessionBase::SetEncryption(SessionEncryption type)
{
    s_enc = type;
}

void SessionBase::SetNextEncryption(SessionEncryption type)
{
    s_next_enc = type;
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


const char* SessionBase::GetPassword()
{
    return s_pwd.c_str();
}

void SessionBase::SetPassword(const char * password)
{
    s_pwd = password;
    INFO("debug", "SESSION: setting password\n");
}

bool SessionBase::HavePassword()
{
    return !s_pwd.empty();
}

void SessionBase::ClearPassword()
{
    s_pwd = "";
}

void SessionBase::GenerateNonce()
{
    GenerateRandomData(s_my_nonce, NONCE_SIZE);
}

bool SessionBase::CheckNonce(const ByteBuffer& buf)
{
    INFO("debug", "SESSIONBASE: my nounce\n");
    s_my_nonce.hexlike();
    INFO("debug", "SESSIONBASE: received nounce\n");
    buf.hexlike();
          
    return s_my_nonce.compare(buf);
}

bool SessionBase::TestRsa()
{
    bool res;
    
    UpdateKeyFilenames();
    
    INFO("debug", "SESSIONBASE: TESTING RSA KEYS\n");
    
    if ((res = RsaTest(f_my_priv_key.c_str(), f_my_pub_key.c_str(), GetPassword())))
        INFO("debug", "SESSIONBASE: RSA TEST SUCCEEDED\n\n");
    else
        INFO("debug", "SESSIONBASE: RSA TEST FAILED\n\n");
    
    return res;
}

void SessionBase::ResetPacketNum() 
{
    u_id_send = 0;
    u_id_receive = 0;
}
