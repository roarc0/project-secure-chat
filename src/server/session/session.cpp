#include "session.h"

Session::Session(Socket* pSock) : 
m_id(0), m_inQueue(false), m_incell(false)
{
    m_Socket = pSock;
}

Session::~Session()
{
    // empty incoming packet queue
    WorldPacket* packet = NULL;
    while (_recvQueue.next(packet))
        delete packet;
}

bool Session::Update(uint32 diff, PacketFilter& updater)
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
                        if (IsInCell())
                        {
                            // We can't transfer if we are in a Cell  
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

bool Session::KickSession()
{
    if (m_Socket)
        m_Socket->CloseSocket();
}

void Session::QueuePacket(Packet* new_packet)
{
    _recvQueue.add(new_packet);
}

void Session::SendPacket(Packet* new_packet)
{
    if (!m_Socket)
        return;

    if (m_Socket->SendPacket(*new_packet) == -1)
        m_Socket->CloseSocket();
}


void Session::SendWaitQueue(int position)
{
    Packet new_packe;
    //TODO settaggio pacchetto che avverte che l'utente è in attesa in posizione position
    pSession->SendPacket(&new_packet);
    
}
