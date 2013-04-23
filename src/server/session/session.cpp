#include "session.h"
#include "networking/opcode.h"
#include "channel.h"
#include "session-manager.h"
#include "network-manager.h"

Session::Session(int pSock) : SessionBase(pSock),
m_id(0), m_inQueue(false), m_channel(NULL)
{

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


bool Session::Update(uint32 /*diff*/, PacketFilter& updater)
{
    Packet* packet = NULL;
    // To prevent infinite loop
    Packet* firstDelayedPacket = NULL;
    // Delete packet after processing by default
    bool deletePacket = true;

    if (m_Socket->IsClosed())
        INFO ("debug", "SESSION: Update socket Chiuso\n");

    while (m_Socket && !m_Socket->IsClosed() &&
            !_recvQueue.empty() && _recvQueue.peek(true) != firstDelayedPacket &&
            _recvQueue.next(packet, updater))
    {
        if (packet->GetOpcode() >= NUM_MSG_TYPES) // Max opcode
        {
            INFO ("debug", "SESSION: Opcode Pacchetto Non Valido\n");
        }
        else
        {
            INFO ("debug", "SESSION: Ricevuto pacchetto\n");
            OpcodeHandler &opHandle = opcodeTable[packet->GetOpcode()];
            try
            {
                switch (opHandle.status)
                {
                    case STATUS_LOGGED:
                        {
                            INFO ("debug", "SESSION: pacchetto in elaborazione\n");
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
                    case STATUS_LOGGING:
                        if (m_inQueue)
                        {
                            INFO ("debug", "Rilevato Packet Injection durante la coda\n");
                            break; // For Packet Injection
                        }
                        (this->*opHandle.handler)(*packet);
                        break;
                    case STATUS_NEVER:
                        INFO ("debug", "SESSION: STATUS_NEVER, pacchetto non elaborato\n");
                        break;
                    case STATUS_UNHANDLED:
                        INFO ("debug", "SESSION: STATUS_UNHANDLED, pacchetto non elaborato\n");
                        break;
                    default:
                        // Log
                        break;
                }
            }
            catch (...)
            {
                INFO ("debug", "Errore Durante Elaborazione Pacchetto\n");
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

void Session::KickSession()
{
    if (m_Socket)
        m_Socket->CloseSocket();
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
    Packet new_packet(SMSG_QUEUE_POSITION, 4);
    new_packet<<uint32(position);
    SendPacket(&new_packet);   
}

int Session::SetNick(std::string& str)
{
    Session_smart ses = s_manager->FindSession(str);
    if (!ses.get())
    {
        return -1;
        //SendSysMessage("Nick già esistente");
    }
    nick = str;
    return 0; 
}

void Session::Handle_Ping(Packet& /*packet*/)
{
    Packet data(SMSG_SYSTEM_MESSAGE, 0);
    data << "Pong";
    SendPacket(&data);
}

void Session::Handle_ServerSide(Packet& /*packet*/)
{
    //LOG
}

void Session::HandleMessage(Packet& packet) 
{
    packet.SetOpcode(SMSG_MESSAGE);

    // Controllo se ci sono comandi
    /*if (ChatHandler(smartThis).ParseCommands(msg.c_str()) > 0)
        return;*/

    //INFO ("debug", "SESSION: Livello Esecuzione Messaggio: %s\n", msg.c_str());

    if (m_channel.get())
    {
        m_channel->SendToAllButOne(&packet, m_id);
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
    std::string c_name = ""; 
    packet >> c_name;
    // prendi password dal pacchetto
    std::string pass = ""; 
    packet >> pass;

    SmartChannel sChan = s_manager->GetChannelMrg()->FindChannel(c_name);

    if (!sChan.get())
    {
        // Notifica all'utente canale non esistente
        SendSysMessage("Canale non Esistente");
        return;
    }

    if (!sChan->CanSessionEnter(smartThis, pass))
    {
        // Invia notifica all'utente che non può entrare nel canale
        SendSysMessage("Password Errata");
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
        SendSysMessage("Errore Aggiunta Canale");
        return;
    }

    setChannel(sChan);
    PSendSysMessage("Entrato nel canale: %s", c_name.c_str());
}

void Session::HandleCreateChannel(Packet& packet) 
{
    // prendi il nome del canale dal pacchetto
    std::string c_name = ""; 
    packet >> c_name;
    // prendi password dal pacchetto
    std::string pass = ""; 
    packet >> pass;

    SmartChannel sChan = s_manager->GetChannelMrg()->CreateChannel(c_name);
    if (!sChan.get())
    {
        SendSysMessage("Canale già esistente");
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
        SendSysMessage("Errore Aggiunta Canale");
        return;
    }

    setChannel(sChan);
    PSendSysMessage("Creato canale: %s", c_name.c_str());
}

void Session::HandleLeaveChannel(Packet& /*packet*/) 
{
    if (!IsInChannel())
    {
        SendSysMessage("Non sei in un canale");
        return;
    }

    // Rimuovere dal canale
    getChannel()->RemoveSession(GetId());
    setChannel(SmartChannel(NULL));

    SendSysMessage("Uscito dal Canale");
}

void Session::HandleListChannel(Packet& /*packet*/) 
{
    std::string info = "";
    s_manager->GetChannelMrg()->getChannelList(info);
    SendSysMessage(info.c_str());
}
