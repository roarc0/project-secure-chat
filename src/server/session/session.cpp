#include "session.h"
#include "networking/opcode.h"
#include "channel.h"
#include "session-manager.h"
#include "network-manager.h"
#include "xml-message.h"

Session::Session(int pSock) : SessionBase(pSock),
m_id(0), m_inQueue(false), m_channel(NULL)
{
    username = "";
    InitKeyUpdateInterval();
    UpdateKeyFilenames();
}

Session::~Session()
{

}

void Session::setSmartPointer(Session_smart m_ses)
{
    smartThis = (Session_smart)m_ses;
}

void Session::deleteSmartPointer()
{
    smartThis = (Session_smart)NULL;
}

int Session::_SendPacket(Packet& pct)
{
    if (!smartThis.get())
        return -1;  
    
    Packet* pkt = new Packet(pct);
    _sendQueue.add(pkt); 
    net_manager->QueueSend(smartThis);
    return 0;
}

int Session::_SendPacket(Packet* pct)
{
    if (!smartThis.get())
        return -1;  
    
    _sendQueue.add(pct); 
    net_manager->QueueSend(smartThis);
    return 0;
}

void Session::InitKeyUpdateInterval()
{
    int r = CFG_GET_INT("key_refresh_interval");
    if ( r < MIN_REFRESH_KEY_INTERVAL )
    {
        INFO("debug", "SESSION: selected %d seconds key refresh interval is too low.\n", r);
        r = MIN_REFRESH_KEY_INTERVAL;
    }
    
    i_timer_key.SetInterval(r * 1000);
    i_timer_key.Reset();
                
    INFO("debug", "SESSION: setting key refresh interval to %d seconds.\n", r);
};

bool Session::Update(uint32 diff, PacketFilter& updater)
{
    Packet* packet = NULL;
    // To prevent infinite loop
    Packet* firstDelayedPacket = NULL;
    // Delete packet after processing by default
    bool deletePacket = true;

    if (m_Socket && !m_Socket->IsClosed() && updater.IsSingleSessionFilter())
        GenerateNewKey(diff);

    while (m_Socket && !m_Socket->IsClosed() &&
            !_recvQueue.empty() && _recvQueue.peek(true) != firstDelayedPacket &&
            _recvQueue.next(packet, updater))
    {
        if (packet->GetOpcode() >= NUM_MSG_TYPES) // Max opcode
        {
            INFO ("debug", "SESSION: opcode is not valid\n");
        }
        else
        {
            INFO ("debug", "SESSION: packet received\n");
            OpcodeHandler &opHandle = opcodeTable[packet->GetOpcode()];
            try
            {
                switch (opHandle.status)
                {
                    case STATUS_LOGGING:
                        {
                            if (m_inQueue)
                            {
                                INFO ("debug", "SESSION: packet injection, while in queue\n");
                                break; // For Packet Injection
                            }

                            INFO ("debug", "SESSION: processing packet\n");
                            (this->*opHandle.handler)(*packet);
                        }
                        break;
                    case STATUS_TRANSFER:
                        if (IsInChannel())
                        {
                            // We can't transfer if we are in a Channel  
                        }
                        else
                        {
                            (this->*opHandle.handler)(*packet);
                        }
                        break;
                    case STATUS_LOGGED:
                        if (!IsAuthenticated())
                        {
                            INFO ("debug", "SESSION: executing packet not authenticated\n");
                            break;
                        }
                        (this->*opHandle.handler)(*packet);
                        break;
                    case STATUS_NEVER:
                        INFO ("debug", "SESSION: STATUS_NEVER, packet hasn't been processed\n");
                        break;
                    case STATUS_UNHANDLED:
                        INFO ("debug", "SESSION: STATUS_UNHANDLED, packet hasn't been processed\n");
                        break;
                    default:
                        // Log
                        break;
                }
            }
            catch (...)
            {
                INFO ("debug", "SESSION: error handling packet\n");
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

void Session::SendSysMessage(const char *str)
{
    INFO ("debug", "SESSION: %s \n", str);
    Packet data(SMSG_SYSTEM_MESSAGE, strlen(str));
    data << str;
    SendPacket(&data);
}

void Session::PSendSysMessage(const char *format, ...)
{
    va_list ap;
    char str [2048];
    va_start(ap, format);
    vsnprintf(str, 2048, format, ap);
    va_end(ap);
    SendSysMessage(str);
}

void Session::SendWaitQueue(int position)
{
    // Invio posizione coda
    Packet data(SMSG_QUEUE_POSITION, 4);
    data << uint32(position);
    SendPacket(&data);   
}

void Session::GenerateNewKey(uint32 diff)
{
    if (s_status != STATUS_AUTHENTICATED)
        return;

    i_timer_key.Update(diff);
    if (!i_timer_key.Passed())
        return;

    INFO ("debug", "SESSION: Generate New Key\n");

    // Scambio chiavi già in corso
    if (u_changekeys == 1) 
        return;

    GenerateRandomKey(s_key_tmp, 32);

    u_changekeys = 0;

    Packet data(SMSG_REFRESH_KEY, 32);
    data.append(s_key_tmp);
    SendPacketToSocket(&data);

    i_timer_key.SetCurrent(0);
}

void Session::UpdateKeyFilenames()
{
       string file = CFG_GET_STRING("rsa_prefix") +
                     CFG_GET_STRING("rsa_my_keys");
       
       f_my_priv_key   = file +  ".pem";
       f_my_pub_key    = file + ".pub";
       
       f_other_pub_key = CFG_GET_STRING("rsa_prefix") +
                         CFG_GET_STRING("rsa_client_pub_key") +
                         *GetUsername() + ".pub";
}

void Session::HandlePing(Packet& /*packet*/)
{
    Packet data(SMSG_SYSTEM_MESSAGE, 0);
    data << "Pong";
    SendPacket(&data);
}

void Session::HandleServerSide(Packet& /*packet*/)
{
    //LOG
}

void Session::HandleMessage(Packet& packet) 
{
    // Controllo se ci sono comandi
    /*if (ChatHandler(smartThis).ParseCommands(msg.c_str()) > 0)
        return;*/

    //INFO ("debug", "SESSION: Executing message: %s\n", msg.c_str());

    if (m_channel.get())
    {
        packet.SetOpcode(SMSG_MESSAGE);
        std::string data;
        packet >> data;
        
        INFO("debug", "USERNAME IS %s\n, DATA %s", GetUsername()->c_str(), data.c_str());
        XMLSetUsernameToMessage(data, *GetUsername());

        Packet pkt(SMSG_MESSAGE, data.size());
        pkt << data;
        
        m_channel->SendToAllButOne(&pkt, m_id);
    }
    else
    {
        SendSysMessage("You have to join or create a channel");
    }
}

void Session::HandleWhisp(Packet& packet) 
{    
    std::string nick;
    packet >> nick;

    Session_smart ses = s_manager->FindSession(nick);
    if (!ses.get())
    {
        SendSysMessage("User not found");
    }

    packet.SetOpcode(SMSG_WHISP);
    ses->SendPacket(&packet);
}

void Session::HandleJoinChannel(Packet& packet) 
{
    // prendi il nome del canale dal pacchetto
    std::string c_name; 
    packet >> c_name;
    // prendi password dal pacchetto
    std::string pass; 
    packet >> pass;

    SmartChannel sChan = s_manager->GetChannelMrg()->FindChannel(c_name);

    if (!sChan.get())
    {
        // Notifica all'utente canale non esistente
        SendSysMessage("Channel does not exists");
        return;
    }

    if (!sChan->CanEnterSession(smartThis, pass))
    {
        // Invia notifica all'utente che non può entrare nel canale
        SendSysMessage("Wrong password");
        return;
    }

    if (IsInChannel())
    {
        // Rimuovere dal canale
        getChannel()->RemoveSession(GetId());
        setChannel(SmartChannel(NULL));
    }

    if (!sChan->AddSession(smartThis))
    {
        SendSysMessage("Error adding channel");
        return;
    }

    setChannel(sChan);
    PSendSysMessage("Joined channel: %s", c_name.c_str());
}

void Session::HandleCreateChannel(Packet& packet) 
{
    // prendi il nome del canale dal pacchetto
    std::string c_name = ""; 
    packet >> c_name;
    // prendi password dal pacchetto
    std::string pass = ""; 
    packet >> pass;

    SmartChannel sChan = s_manager->GetChannelMrg()->CreateChannel(c_name, pass);
    if (!sChan.get())
    {
        SendSysMessage("Channel already exists");
        return;
    }

    if (IsInChannel())
    {
        // Rimuovere dal canale
        getChannel()->RemoveSession(GetId());
        setChannel(SmartChannel(NULL));
    }

    if (!sChan->AddSession(smartThis))
    {
        SendSysMessage("Error adding channel");
        return;
    }

    setChannel(sChan);
    PSendSysMessage("Creating new channel: %s", c_name.c_str());
}

void Session::HandleLeaveChannel(Packet& /*packet*/) 
{
    if (!IsInChannel())
    {
        SendSysMessage("You aren't in any channel");
        return;
    }

    getChannel()->RemoveSession(GetId());
    setChannel(SmartChannel(NULL));

    SendSysMessage("Channel left");
}

void Session::HandleListChannel(Packet& /*packet*/) 
{
    std::string info;
    s_manager->GetChannelMrg()->getChannelList(info);
    SendSysMessage(info.c_str());
}

void Session::HandleLogin(Packet& packet)
{
    INFO ("debug", "SESSION: LOGIN procedure status: %d\n", s_status);

    switch (s_status)
    {
        case STATUS_CONNECTED:
            {
                SetSessionStatus(STATUS_LOGIN_STEP_1);
                SetNextEncryption(ENC_RSA);
                Packet data(SMSG_LOGIN, 0);
                SendPacket(&data);
            }
            break;
        case STATUS_LOGIN_STEP_1:
            {
                std::string user, pwd;
                bool valid = true;
                    
                packet >> user;
                packet >> pwd;
                
                valid = s_manager->FindSession(user).is_null();

                if (!valid)
                    INFO("debug","SESSION: username \"%s\" is already loggedin\n", user.c_str());
                else
                {
                    valid = SetUsername(user);
                    if (!valid) 
                        INFO("debug","SESSION: username \"%s\" is not valid\n", user.c_str());
                    else
                    {
                        valid = db_manager->UserExists(user);
                        
                        if (!valid)
                            INFO("debug","SESSION: username \"%s\" doesn't exist\n", user.c_str());
                    }
                }

                if (valid)
                {
                    INFO("debug", "SESSION: username \"%s\" accepted\n", user.c_str());
                    SetSessionStatus(STATUS_AUTHENTICATED);
                    UpdateKeyFilenames();  // locate user public key.
                }
                else
                    SetSessionStatus(STATUS_REJECTED);
                
                Packet data(SMSG_LOGIN, 1);
                data << (uint8) valid;                
                SendPacket(&data);
            }
            break;
            
        case STATUS_AUTHENTICATED:
            {
                GenerateRandomKey(s_key, 32);

                Packet data(SMSG_LOGIN, 32);
                data.append(s_key);
                SendPacketToSocket(&data); // TODO inserire SendPacket e attivare la criptazione al passo dopo
                
                Xor(s_key, packet);
                SetEncryption(s_key, ENC_AES256);
                
                INFO("debug", "SESSION: AES key established\n");
                
                s_manager->GetChannelMrg()->JoinDefaultChannel(smartThis);
            }
        break;
        
        case STATUS_REJECTED:
        default:
            {

            }
        break;
    }
}

void Session::HandleRefreshKey(Packet& packet)
{
    INFO ("debug", "SESSION: handle refresh AES key\n");

    if (u_changekeys != 0) // Packet Injection
        return;
    
    Xor(s_key_tmp, (const ByteBuffer) packet);

    u_changekeys = 1;

    Packet data(SMSG_REFRESH_KEY, 0);
    SendPacketToSocket(&data);
}
