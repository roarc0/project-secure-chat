#include "session.h"
#include "networking/opcode.h"
#include "chat-handler.h"
#include "client-core.h"

Session::Session()
{
    username = CFG_GET_STRING("username");
    c_Socket = new SocketClient(SOCK_STREAM, 0);
    m_Socket = (SocketBase*) c_Socket;
}

Session::~Session()
{
    delete c_Socket;
}

int Session::_SendPacket(Packet&)
{ 
    INFO("debug","SESSION: _SendPacket must not be called here\n");
    return 0; 
}

bool Session::Connect()
{
    string host = CFG_GET_STRING("server_host");
    int port = CFG_GET_INT("server_port");
    
    try
    {
        c_Socket->Connect(host, port);
        SetConnected(true);                  
        SendToGui("", 'e', "Connected to %s:%d!\n", host.c_str(), port); 
    }
    catch(SocketException &e)
    {
        INFO("debug", "SESSION: connection failed %s:%d (%s)\n",
             host.c_str(), port, e.what());
        SetConnected(false);
        SendToGui("", 'e', "Connection failed %s:%d!\n", 
                  host.c_str(), port);   
        return false;
    }
    catch(...)
    {
        INFO("debug", "SESSION: connect default exception\n");
        return false;
    }

    INFO("debug", "SESSION: connection successful %s:%d\n",
         host.c_str(), port);

    u_id_send = 0;
    u_id_receive = 0;

    return true;
}

bool Session::Disconnect()
{
    string host = CFG_GET_STRING("server_host");
    int port = CFG_GET_INT("server_port");

    try
    {
        c_Socket->Disconnect();
    }
    catch(SocketException &e)
    {
        INFO("debug", "SESSION: disconnection failed %s:%d (%s)\n",
              host.c_str(), port, e.what());
        SendToGui("",'e', "Disconnection failed?!\n");

        return false;
    }
    
    SetConnected(false);
    INFO("debug", "SESSION: disconnection successful %s:%d\n",
          host.c_str(), port);  
         
    SendToGui("", 'e', "Disconnected from %s:%d!\n", host.c_str(), port); 
    
    return true;
}

void Session::UpdateKeyFilenames()
{
       string file = CFG_GET_STRING("rsa_prefix") +
                     CFG_GET_STRING("rsa_my_keys") +
                     *(GetUsername());
       
       f_my_priv_key = file + ".pem";
       f_my_pub_key  = file + ".pub";
       
       f_other_pub_key = CFG_GET_STRING("rsa_prefix") +
                         CFG_GET_STRING("rsa_server_pub_key") + ".pub";
}

void Session::ResetSocket()
{
    delete c_Socket;
    c_Socket = new SocketClient(SOCK_STREAM, 0);
    m_Socket = (SocketBase*) c_Socket;
}

void Session::SendToGui(std::string nick, char type, std::string msg)
{
    bool timestamp = ((type == 'e') ? false:true);
    c_core->AddMessage(msg, nick, type, timestamp);
    c_core->SignalEvent();
}

void Session::SendToGui(std::string nick, char type, const char * fmt, ...)
{
    int ret;
    char* buffer;
    va_list ap;
    va_start(ap, fmt);
    ret = vasprintf(&buffer, fmt, ap);
    va_end(ap);

    if (ret)
    {
        SendToGui(nick, type, (string) buffer);
        free(buffer);
    }
    else
        perror("vasprintf");
}

bool Session::HandleSend(const char* msg)
{
    if (!IsConnected())
        return false;

    INFO("debug","SESSION: sending message: %s\n", msg);

    if (ChatHandler(this).ParseCommands(msg) > 0)
        return true;

    Packet pack(CMSG_MESSAGE);
    pack << XMLBuildMessage(GetUsername()->c_str(), msg);
    
    SendPacketToSocket(&pack);

    return true;
}

bool Session::Update()
{
    Packet* packet = RecvPacketFromSocket();

    if (!packet)
        return false;

    if (packet->GetOpcode() >= NUM_MSG_TYPES)
    {
        INFO ("debug", "SESSION: opcode is not valid\n");
    }
    else
    {
        OpcodeHandler &opHandle = opcodeTable[packet->GetOpcode()];
        try
        {
            switch (opHandle.status)
            {
                case STATUS_LOGGING:
                {
                    INFO ("debug", "SESSION: login procedure status is: %d\n", GetSessionStatus()); 
                    (this->*opHandle.handler)(*packet);                   
                }    
                break;
                case STATUS_LOGGED:
                {
                    if (IsAuthenticated())
                        (this->*opHandle.handler)(*packet);
                    else
                        INFO ("debug", "SESSION: executing packet not authenticated\n");
                }
                break;
                case STATUS_NEVER:
                    INFO ("debug", "SESSION: header is NULL\n");
                break;
                case STATUS_UNHANDLED:

                break;
                default:

                break;
            }
        }
        catch (...)
        {
            INFO ("debug", "SESSION: default exception, packet elaboration error\n");  // TODO
        }
    }

    delete packet;
    
    if (!m_Socket || m_Socket->IsClosed())
        return false;

    return true;
}

bool Session::Update(uint32 /*diff*/)
{
    Packet* packet = NULL;
    bool deletePacket = true;

    while (m_Socket && !m_Socket->IsClosed() &&
            !_recvQueue.empty() && _recvQueue.next(packet))
    {
        if (packet->GetOpcode() >= NUM_MSG_TYPES)
        {
            INFO ("debug", "SESSION: opcode is not valid\n");
        }
        else
        {
            OpcodeHandler &opHandle = opcodeTable[packet->GetOpcode()];
            try
            {
                switch (opHandle.status)
                {
                    case STATUS_LOGGING:
                    {
                        INFO ("debug", "SESSION: login procedure status is: %d\n", GetSessionStatus());
                    }    
                    break;
                    case STATUS_LOGGED:
                    {
                        (this->*opHandle.handler)(*packet);
                    }
                    break;
                    case STATUS_NEVER:
                        INFO ("debug", "SESSION: never!\n");
                    break;
                    case STATUS_UNHANDLED:
                        INFO ("debug", "SESSION: session message not handled\n");
                    break;
                    default:
                        INFO ("debug", "SESSION: session? what is a session?\n");
                    break;
                }
            }
            catch (...)
            {
                INFO ("debug", "SESSION: packet elaboration error\n");  // TODO

            }
        }

        if (deletePacket)
            delete packet;
    }

    if (!m_Socket || m_Socket->IsClosed())
        return false;

    return true;
}

void Session::HandleClientSide(Packet& /*packet*/)
{
    INFO ("debug", "SESSION: receiving packet from another client!!\n");
}

void Session::HandlePing(Packet& /*packet*/)
{
    INFO ("debug", "SESSION: receiving ping!\n");
}

void Session::HandleMessage(Packet& packet)
{
    INFO ("debug", "SESSION: handling message\n");
    std::string msg, user;
    XMLReadMessage((const char*)packet.contents(), user, msg);
    SendToGui(user, 'm', msg);
}

void Session::HandleWhisp(Packet& packet)
{
    INFO ("debug", "SESSION: handling whisp\n");
    std::string msg, user;
    XMLReadMessage((const char*)packet.contents(), user, msg);
    SendToGui(user, 'w', msg);
} 

void Session::HandleServerMessage(Packet& packet)
{
    INFO ("debug", "SESSION: handle server message\n");
    SendToGui("", 'e', (const char*)packet.contents());
}

void Session::HandleJoinChannel(Packet& packet)
{
    INFO ("debug", "SESSION: handle join message\n");
    std::string name, status, msg;
    XMLReadUpdate((const char*)packet.contents(), name, status);
    msg = "joined the channel";
    SendToGui(name, 'j', (const char*)msg.c_str());    
}

void Session::HandleLeaveChannel(Packet& packet)
{
    INFO ("debug", "SESSION: handle leave message\n");
    std::string name, status, msg;
    XMLReadUpdate((const char*)packet.contents(), name, status);
    msg = "left the channel";
    SendToGui(name, 'l', (const char*)msg.c_str());    
}

void Session::HandleRefreshKey(Packet& packet)
{
    INFO ("debug", "SESSION: handle refresh key\n");

    if (packet.size() == 0)
    {
       SetEncryption(s_key_tmp, ENC_AES256);

       // Resettare numerazione pacchetti
       ResetPacketNum();
       
       // TODO riattivare send
    }
    else if (packet.size() == 32)
    {
        ByteBuffer s_key2;
        GenerateRandomKey(s_key2, 32);

        Packet data(CMSG_REFRESH_KEY, 32);
        data.append(s_key2);
        SendPacketToSocket(&data);  
        
        Xor(s_key2, (const ByteBuffer) packet);
        
        s_key_tmp.clear();
        s_key_tmp.append(s_key2);
        
        // TODO Bloccare send fino alla ricezione del pacchetto di ritorno
    }
    else
        INFO ("debug", "SESSION: refresh key, corrupted packet\n"); 
}

void Session::HandleLogin(Packet& packet)
{
    INFO ("debug", "SESSION: hanle login message\n");
    
    switch (GetSessionStatus())
    {
        case STATUS_CONNECTED:
            {
                SetSessionStatus(STATUS_LOGIN_STEP_1);
                INFO("debug", "SESSION: sending first RSA encrypted packet\n");
                SetEncryption(ENC_RSA);
                
                Packet data(CMSG_LOGIN, 0);
                data << *GetUsername(); /* check size */
                
                string pwd_digest;
                SHA256_digest(GetPassword(), pwd_digest);
                data << pwd_digest;
                
                uint8 nonce[NONCE_SIZE];
                
                packet.read(nonce, NONCE_SIZE);
                s_other_nonce.clear();
                s_other_nonce.append(nonce, NONCE_SIZE);
                data.append(s_other_nonce);
                
                GenerateNonce();
                data.append(s_my_nonce);
                
                INFO("debug", "\n\nSESSION: AUTH PACKET >>>>> \n\n");
                data.hexlike();
                INFO("debug", "\n\n");
                
                SendPacketToSocket(&data);
            }   
            break;
        case STATUS_LOGIN_STEP_1:
            {
                string response;
                packet >> response;
                if(response.compare("authenticated") == 0)
                {
                    uint8 nonce[NONCE_SIZE];
                    ByteBuffer read_nonce;
                    
                    packet.read(nonce, NONCE_SIZE);
                    read_nonce.append(nonce, NONCE_SIZE);
                    
                    if(CheckNonce(read_nonce))
                    {
                        SendToGui("", 'e', "Login succeeded!");
                        SetSessionStatus(STATUS_AUTHENTICATED);
                        
                        GenerateRandomKey(s_key, 32);
                        Packet data(CMSG_LOGIN, 32);
                        data.append(s_key);
                        SendPacketToSocket(&data);
                    }
                    else
                    {
                       SetSessionStatus(STATUS_REJECTED);
                       SendToGui("", 'e', "Fake Server!");
                    }
                }
                else
                {
                    SendToGui("", 'e', "Login failed!");    
                    SetSessionStatus(STATUS_REJECTED);
                    m_Socket->CloseSocket();
                    ResetSocket();
                }
            }
            break;
        case STATUS_AUTHENTICATED:   
            {
                Xor(s_key, (const ByteBuffer) packet);
                SetEncryption(s_key, ENC_AES256);
                ResetPacketNum();
                INFO("debug", "SESSION: AES key established\n");
            }
            break;
        default:
        
        break;
    }
 
}

void Session::HandleQueuePos(Packet& packet)
{
    uint32 pos;
    packet >> pos;
    SendToGui("", 'e', "Queue position is %u\n", pos);
}

void Session::HandleChannelUsersList(Packet& packet)
{
    uint32 size;
    std::string name;

    packet >> size;

    SendToGui("", 'L', ""); 

    for (uint32 i = 0; i < size; i++)
    {   
        name = "";
        packet >> name;
        SendToGui(name, 'J', ""); 

    }
}
