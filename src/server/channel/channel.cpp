#include "channel.h"
#include "networking/opcode.h"

Channel::Channel(std::string& c_name, std::string& c_pass) : name(c_name), pass(c_pass), b_todelete(false)
{
    INFO ("debug", "CHANNEL: Channel constructor\n");
}

Channel::~Channel()
{
    INFO ("debug", "CHANNEL: Channel destructor\n");
}

void Channel::Update(uint32 t_diff)
{
    //Update Sessions
    for (mapSession::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        Session_smart pSession = itr->second;
        MapSessionFilter updater(pSession.get());
        pSession->Update(t_diff, updater);
    }
}

bool Channel::DelayedUpdate(uint32 /*t_diff*/)
{
    return m_sessions.size() ? true : (name == "Default" ? true : false);
}

bool Channel::CanEnterSession(Session_smart /*ses*/, std::string& c_pass) const
{ 
    return c_pass == pass;
}

Session_smart Channel::FindSession(uint32 id)
{
    Lock guard(m_mutex);

    mapSession::const_iterator iter = m_sessions.find(id);
    return (iter == m_sessions.end() ? Session_smart(NULL) : iter->second);
}

bool Channel::AddSession(Session_smart ses)
{
    m_mutex.Acquire();
    m_sessions.insert(mapSession_pair(ses->GetId(), ses));
    m_mutex.Release();

    // Invio ingresso nel canale
    Packet new_packet(SMSG_JOIN_CHANNEL, 0);
    new_packet << XMLBuildUpdate(ses->GetUsername()->c_str(), "join");
    SendToAllButOne(&new_packet, ses->GetId());

    SendUsersList(ses);
    return true;
}

int Channel::getSessionNumer()
{
    Lock guard(m_mutex);
    return m_sessions.size();
}

bool Channel::RemoveSession(uint32 id)
{
    m_mutex.Acquire();

    mapSession::iterator iter = m_sessions.find(id);
    if (iter != m_sessions.end())
    {
        Session_smart ses = iter->second;
        m_sessions.erase(iter);     
        m_mutex.Release();

        // Invio uscita dal canale
        Packet packet(SMSG_LEAVE_CHANNEL, 0);
        packet << XMLBuildUpdate(ses->GetUsername()->c_str(), "leave");
        SendToAllButOne(&packet, ses->GetId());

        Packet packet2(SMSG_USERS_CHANNEL_LIST, 4);
        packet2 << (uint32)0;
        ses->SendPacket(&packet2);

        return true;
    }
    m_mutex.Release();
    return false;
}

int Channel::SetName(std::string& c_name)
{
    Lock guard(m_mutex);

    name = c_name;

    Packet new_packet;
    MakeChannelChangeName(&new_packet);
    SendToAll(&new_packet);

    return 0;
}

void Channel::SendToAll(Packet* packet)
{
    for (mapSession::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        itr->second->SendPacket(packet);
    }
}

void Channel::SendToAllButOne(Packet* packet, uint32 id)
{
    for (mapSession::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        Session_smart pSession = itr->second;
        if (pSession->GetId() != id)
            pSession->SendPacket(packet);
    }
}

void Channel::SendToOne(Packet* packet, uint32 id)
{
    Session_smart ses = FindSession(id);
    if (ses.get())
        ses->SendPacket(packet);
}

void Channel::MakeChannelChangeName(Packet* /*packet*/)
{
    // Settaggio pacchetto
}

void Channel::SendUsersList(Session_smart ses)
{
    Packet packet(SMSG_USERS_CHANNEL_LIST, 0);
    packet << (uint32)m_sessions.size();
    for (mapSession::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        packet << *(itr->second->GetUsername());
    }    
    ses->SendPacket(&packet);
}
