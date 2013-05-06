#include "session.h"
#include "networking/opcode.h"
#include "chat-handler.h"
#include "client-core.h"

Session::Session() //: SessionBase()
{
    username = CFG_GET_STRING("username");
    c_Socket = new SocketClient(SOCK_STREAM, 0);
    m_Socket = (SocketBase*) c_Socket;
}

Session::~Session()
{
    delete c_Socket;
}

bool Session::Connect()
{
    /* HORROR SHOW */
    stringstream ss;
    ss << "Connected to " << CFG_GET_STRING("server_host")
       << ":" << CFG_GET_INT("server_port");
    string str_connect = ss.str();

    try
    {
        c_Socket->Connect(CFG_GET_STRING("server_host"),
                          CFG_GET_INT("server_port"));
        SetConnected(true);                  
        SendToGui(str_connect.c_str(), "",'e');
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
        INFO("debug", "SESSION: default exception\n");
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
    SendToGui("Disconnected!\n", "",'e');
    
    return true;
}

const char* Session::GetPassword()
{
    return m_pwd_digest.c_str();
}

void Session::SetPassword(const char * password)
{
    string pwd_digest;
    assert(password);
    SHA256_digest(password, strlen(password), pwd_digest);
    m_pwd_digest = pwd_digest;
    INFO("debug", "SESSION: setting password digest: %s\n", pwd_digest.c_str());
}

bool Session::IsPasswordSet()
{
    return !m_pwd_digest.empty();
}

void Session::ClearPassword()
{
    m_pwd_digest = "";
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

    if (packet->GetOpcode() >= NUM_MSG_TYPES) // Max opcode
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
            INFO ("debug", "SESSION: packet elaboration error\n");
            // TODO
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
    // Delete packet after processing by default
    bool deletePacket = true;

    while (m_Socket && !m_Socket->IsClosed() &&
            !_recvQueue.empty() && _recvQueue.next(packet))
    {
        if (packet->GetOpcode() >= NUM_MSG_TYPES) // Max opcode
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
                INFO ("debug", "SESSION: packet elaboration error\n");
                // TODO
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
    INFO ("debug", "SESSION: Handling Message\n");
    std::string msg, user;
    XMLReadMessage((const char*)packet.contents(), user, msg);
    SendToGui(msg, user, 'm');
}

void Session::HandleServerMessage(Packet& packet)
{
    INFO ("debug", "SESSION: Handle Server Message\n");
    SendToGui((const char*)packet.contents(), "", 'e');
}

void Session::HandleJoinChannel(Packet& packet) // TODO usare un solo handle e controllare status
{
    INFO ("debug", "SESSION: Handle Join Message\n");
    std::string name, status, msg;
    XMLReadUpdate((const char*)packet.contents(), name, status);
    msg = "joined the channel";
    SendToGui((const char*)msg.c_str(), name, 'j');    
}

void Session::HandleLeaveChannel(Packet& packet)
{
    INFO ("debug", "SESSION: Handle Leave Message\n");
    std::string name, status, msg;
    XMLReadUpdate((const char*)packet.contents(), name, status);
    msg = "left the channel";
    SendToGui((const char*)msg.c_str(), name, 'l');    
}

void Session::HandleLogin(Packet& packet)
{
    INFO ("debug", "SESSION: LOGIN procedure\n");    
    Packet data(CMSG_LOGIN, 0);
    string xml;
    bool valid;
    
    switch (GetSessionStatus())
    {
        case STATUS_CONNECTED:
            {
                
                data << XMLBuildLogin(GetUsername()->c_str(), GetPassword());

                SendPacketToSocket(&data);
                SetSessionStatus(STATUS_LOGIN_STEP_1);
            }
            break;
        case STATUS_LOGIN_STEP_1:
            {
                packet >> xml;
                XMLReadLoginResponse(xml.c_str(), valid);
                if(valid)
                {
                    SendToGui("Login succeeded!", "", 'e'); 
                    SetSessionStatus(STATUS_AUTHENTICATED);
                    SendPacketToSocket(&data); // ack and join default channel
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
        default:
        
        break;
    }
 
}
