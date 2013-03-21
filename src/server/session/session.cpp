#include "session.h"
#include "opcode.h"
#include "channel.h"
#include "session-manager.h"

Session::Session(SocketBase* pSock) : SessionBase(pSock),
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
            // LOG
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


void Session::SendWaitQueue(int /*position*/)
{
    Packet new_packet;
    //TODO settaggio pacchetto che avverte che l'utente è in attesa in posizione position
    SendPacket(&new_packet);   
}

void Session::Handle_ServerSide(Packet& /*packet*/)
{
    //LOG
}

void Session::HandleMessage(Packet& /*packet*/) 
{
    //TODO
}

void Session::HandleJoinChannel(Packet& /*packet*/) 
{
    if (channel_name != "")
        return;
    
    std::string c_name = ""; // prendi il nome del canale dal pacchetto
    std::string pass = ""; // prendi password dal pacchetto

    Channel* pChan = s_manager->GetChannelMrg()->FindChannel(c_name);

    if (pChan)
    {
        // Notifica all'utente canale non esistente
        return;
    }

    if (!pChan->CanSessionEnter(this, pass))
    {
        // Invia notifica all'utente che non può entrare nel canale
        return;
    }

    if (!pChan->AddSession(this))
    {
        // Errore aggiunta canale
        return;
    }
}
