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
    stringstream ss;
    ss << "Connected to " << CFG_GET_STRING("server_host")
       << ":" << CFG_GET_INT("server_port");

    try
    {
        c_Socket->Connect(CFG_GET_STRING("server_host"),
                          CFG_GET_INT("server_port"));
        SetConnected(true);                  
        SendToGui(ss.str(), "",'e');
    }
    catch(SocketException &e)
    {
        INFO("debug", "SESSION: connection failed %s:%d (%s)\n",
             CFG_GET_STRING("server_host").c_str(),
             CFG_GET_INT("server_port"), e.what());
        SetConnected(false);
        SendToGui("Connection failed!\n", "",'e');   
        return false;
    }
    catch(...)
    {
        INFO("debug", "SESSION: connect default exception\n");
        return false;
    }

    INFO("debug", "SESSION: connection successful %s:%d\n",
         CFG_GET_STRING("server_host").c_str(),
         CFG_GET_INT("server_port"));

    return true;
}

bool Session::Disconnect()
{
    try
    {
        c_Socket->Disconnect();
    }
    catch(SocketException &e)
    {
        INFO("debug", "SESSION: disconnection failed %s:%d (%s)\n",
             CFG_GET_STRING("server_host").c_str(),
             CFG_GET_INT("server_port"), e.what());
        SendToGui("Disconnection failed?!\n", "",'e');

        return false;
    }
    
    SetConnected(false);
    INFO("debug", "SESSION: disconnection successful %s:%d\n",
         CFG_GET_STRING("server_host").c_str(),
         CFG_GET_INT("server_port"));  
         
    stringstream ss;
    ss << "Disconnected from " << CFG_GET_STRING("server_host")
       << ":" << CFG_GET_INT("server_port");
    string str_disconnect = ss.str();
    
    SendToGui(str_disconnect.c_str(), "",'e');
    
    return true;
}

const char* Session::GetPassword()
{
    return s_pwd.c_str();
}

void Session::SetPassword(const char * password)
{
    s_pwd = password;
    INFO("debug", "SESSION: setting password\n");
}

bool Session::HavePassword()
{
    return !s_pwd.empty();
}

void Session::ClearPassword()
{
    s_pwd = "";
}

void Session::UpdateKeyFilenames()
{
       string file = CFG_GET_STRING("rsa_prefix") +
                     CFG_GET_STRING("rsa_my_keys") +
                     *(GetUsername());
       f_key_priv = file + ".pem";
       f_key_pub  = file + ".pub";
}

bool Session::TestRsa()
{
    bool res;
    
    UpdateKeyFilenames();
    
    INFO("debug", "CLIENT_CORE: TESTING RSA KEYS\n");
    
    if(res = RsaTest(f_key_priv.c_str(), f_key_pub.c_str(), GetPassword()))
        INFO("debug", "CLIENT_CORE: RSA TEST SUCCEEDED\n\n");
    else
        INFO("debug", "CLIENT_CORE: RSA TEST FAILED\n\n");
    
    return res;
}

void Session::ResetSocket()
{
    delete c_Socket;
    c_Socket = new SocketClient(SOCK_STREAM, 0);
    m_Socket = (SocketBase*) c_Socket;
}

void Session::SendToGui(std::string msg, std::string nick, char type)
{
    bool timestamp = ((type == 'e') ? false:true);
    c_core->AddMessage(msg, nick, type, timestamp);
    c_core->SignalEvent();
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
    SendToGui(msg, user, 'm');
}

void Session::HandleServerMessage(Packet& packet)
{
    INFO ("debug", "SESSION: handle server message\n");
    SendToGui((const char*)packet.contents(), "", 'e');
}

void Session::HandleJoinChannel(Packet& packet)
{
    INFO ("debug", "SESSION: handle join message\n");
    std::string name, status, msg;
    XMLReadUpdate((const char*)packet.contents(), name, status);
    msg = "joined the channel";
    SendToGui((const char*)msg.c_str(), name, 'j');    
}

void Session::HandleLeaveChannel(Packet& packet)
{
    INFO ("debug", "SESSION: handle leave message\n");
    std::string name, status, msg;
    XMLReadUpdate((const char*)packet.contents(), name, status);
    msg = "left the channel";
    SendToGui((const char*)msg.c_str(), name, 'l');    
}

void Session::HandleUpdateKey(Packet& packet)
{
    INFO ("debug", "SESSION: Handle update key\n");
    
    ByteBuffer s_key;
    GenerateRandomKey(s_key, 32);
    
    /*Packet data(CMSG_UPDATEKEY, 32);
    data.append(s_key);
    SendPacketToSocket(&data);  
    
    Xor(s_key, (const ByteBuffer) packet);
    SetEncryption(s_key, ENC_AES256);*/
}

void Session::HandleLogin(Packet& packet)
{
    INFO ("debug", "SESSION: hanle login message\n");    

    bool valid;
    
    switch (GetSessionStatus())
    {
        case STATUS_CONNECTED:
            {
                SetSessionStatus(STATUS_LOGIN_STEP_1);
                
                Packet data(CMSG_LOGIN, 0);
                data << *GetUsername();
                data << GetPassword();
                SendPacketToSocket(&data);
            }
            break;
        case STATUS_LOGIN_STEP_1:
            {
                packet >> valid;
                if(valid)
                {
                    SendToGui("Login succeeded!", "", 'e');
                    SetSessionStatus(STATUS_AUTHENTICATED);
                    
                    GenerateRandomKey(s_key, 32);
                    Packet data(CMSG_LOGIN, 32);
                    data.append(s_key);
                    SendPacketToSocket(&data);
                }
                else
                {
                    SendToGui("Login failed!", "", 'e');    
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
                
                INFO("debug", "SESSION: key established\n");
            }
            break;
        default:
        
        break;
    }
 
}
