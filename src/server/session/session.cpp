#include "session.h"
#include "opcode.h"
#include "channel.h"
#include "session-manager.h"
#include "chat_handler.h"

Session::Session(int pSock) : SessionBase(pSock),
m_id(0), m_inQueue(false), channel_name("")
{

}

Session::~Session()
{

}

bool Session::Update(uint32 /*diff*/, PacketFilter& updater)
{
    Packet* packet = NULL;
    // To prevent infinite loop
    Packet* firstDelayedPacket = NULL;
    // Delete packet after processing by default
    bool deletePacket = true;

    while (m_Socket && !m_Socket->IsClosed() &&
            !_recvQueue.empty() && _recvQueue.peek(true) != firstDelayedPacket &&
            _recvQueue.next(packet, updater))
    {
        if (packet->GetOpcode() >= NUM_MSG_TYPES) // Max opcode
        {
            INFO ("debug", "Opcode Pacchetto Non Valido\n");
        }
        else
        {
            OpcodeHandler &opHandle = opcodeTable[packet->GetOpcode()];
            try
            {
                switch (opHandle.status)
                {
                    case STATUS_LOGGED:
                        {
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
                        // Log
                        break;
                    case STATUS_UNHANDLED:
                        // Log
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

void Session::SendWaitQueue(int position)
{
    // Invio posizione coda
    Packet new_packet(SMSG_QUEUE_POSITION, 4);
    new_packet<<uint32(position);
    SendPacket(&new_packet);   
}

void Session::Handle_Ping(Packet& packet)
{

}

void Session::Handle_ServerSide(Packet& /*packet*/)
{
    //LOG
}

void Session::HandleMessage(Packet& packet) 
{
    std::string msg;
    packet >> msg;

    // Controllo se ci sono comandi
    if (ChatHandler(this).ParseCommands(msg.c_str()) > 0)
        return;

    INFO ("debug", "Livello Esecuzione Messaggio: %s \n", msg.c_str());

    // Test send non funzionerà mai
    Packet respacket;
    respacket << "Il Server ha ascoltato il tuo messaggio e lo ha ingorato brutalmente.";
    SendPacket(&respacket);
    //TODO
}

void Session::HandleJoinChannel(Packet& packet) 
{
    if (channel_name != "")
    {
        // sono già in un canale
        return;
    }

    // prendi il nome del canale dal pacchetto
    std::string c_name = ""; 
    packet >> c_name;
    // prendi password dal pacchetto
    std::string pass = ""; 
    packet >> pass;

    Channel* pChan = s_manager->GetChannelMrg()->FindChannel(c_name);

    if (pChan)
    {
        // Notifica all'utente canale non esistente
        Packet pkt(SMSG_CHANNEL_NOTIFY);

        pkt << "Canale non Esistente";
        SendPacket(&pkt);        
        return;
    }

    if (!pChan->CanSessionEnter(this, pass))
    {
        // Invia notifica all'utente che non può entrare nel canale
        Packet pkt(SMSG_CHANNEL_NOTIFY);
        pkt << "Password Errata";
        SendPacket(&pkt);  
        return;
    }

    if (!pChan->AddSession(this))
    {
        Packet pkt(SMSG_CHANNEL_NOTIFY);
        pkt << "Errore Aggiunta Canale";
        SendPacket(&pkt); 
        return;
    }
}
