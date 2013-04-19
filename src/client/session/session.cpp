#include "session.h"
#include "networking/opcode.h"
#include "chat-handler.h"
#include "client-core.h"

Session::Session() //: SessionBase()
{
    connected = false;
    c_Socket = new SocketClient(SOCK_STREAM, 0);
    m_Socket = (SocketBase*) c_Socket;
}

Session::~Session()
{
    // TODO
}

bool Session::Connect()
{
    try
    {
        c_Socket->Connect(CFG_GET_STRING("server_host"),
                          CFG_GET_INT("server_port"));
    }
    catch(SocketException &e)
    {
        INFO("debug", "connection failed     %s:%d (%s)\n",
             CFG_GET_STRING("server_host").c_str(),
             CFG_GET_INT("server_port"), e.what());
        SetConnected(false);

        return false;
    }

    INFO("debug", "connection successful %s:%d\n",
         CFG_GET_STRING("server_host").c_str(),
         CFG_GET_INT("server_port"));
    SetConnected(true);

    return true;
}

bool Session::Disconnect()
{
    try
    {
        c_Socket->Disconnect();
        SetConnected(false);
    }
    catch(SocketException &e)
    {
        INFO("debug", "disconnection failed     %s:%d (%s)\n",
             CFG_GET_STRING("server_host").c_str(),
             CFG_GET_INT("server_port"), e.what());

        return false;
    }

    INFO("debug", "disconnection successful %s:%d\n",
         CFG_GET_STRING("server_host").c_str(),
         CFG_GET_INT("server_port"));

    return true;
}

void Session::ResetSocket()
{
    delete c_Socket;
    c_Socket = new SocketClient(SOCK_STREAM, 0);
    m_Socket = (SocketBase*) c_Socket;
}

void Session::SendToGui(std::string str)
{
    c_core->messages.add(str);
    c_core->SignalEvent();
}

bool Session::HandleSend(const char* msg)
{
    if (!IsConnected())
        return false;

    INFO("debug","sending message: %s\n", msg);

    if (ChatHandler(this).ParseCommands(msg) > 0)
        return true;

    Packet pack(CMSG_MESSAGE);
    pack << xmsg.BuildMessage(CFG_GET_STRING("nickname").c_str(), msg);
    
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
                case STATUS_NEVER:
                    INFO ("debug", "Header NULL\n");
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

void Session::Handle_ClientSide(Packet& /*packet*/)
{
    INFO ("debug", "Sto ricevendo un pacchetto di un altro client!\n");
}

void Session::Handle_Ping(Packet& /*packet*/)
{

}

void Session::HandleMessage(Packet& packet)
{
    INFO ("debug", "Handle Message\n");

    std::string str;
    packet >> str;
    SendToGui(xmsg.ReadMessage(str.c_str()));
}

void Session::HandleServerMessage(Packet& packet)
{
    INFO ("debug", "Handle Server Message\n");

    std::string str;
    packet >> str;
    SendToGui(str);
}

