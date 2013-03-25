#include "channel.h"

Channel::Channel(std::string& c_name) : name(c_name), b_todelete(false)
{

}

Channel::~Channel()
{

}

void Channel::Update(uint32 t_diff)
{
    //Update Sessions
    for (mapSession::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        Session* pSession = itr->second;
        MapSessionFilter updater(pSession);
        pSession->Update(t_diff, updater);
    }
}

bool Channel::DelayedUpdate(uint32 /*t_diff*/)
{
    return b_todelete ? false : true;
}

bool Channel::CanSessionEnter(Session* /*ses*/, std::string& /*pass*/) const 
{ 
    // TODO
    return true; 
}

Session* Channel::FindSession(uint32 id)
{
    mapSession::const_iterator iter = m_sessions.find(id);
    return (iter == m_sessions.end() ? NULL : iter->second);
}

bool Channel::AddSession(Session* ses)
{
    Lock guard(m_mutex);
    m_sessions.insert(mapSession_pair(ses->GetId(), ses));
    return true;
}

bool Channel::RemoveSession(uint32 id)
{
    Lock guard(m_mutex);

    mapSession::iterator iter = m_sessions.find(id);
    if (iter != m_sessions.end())
    {
        m_sessions.erase(iter);
        return true;
    }
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
        Session* pSession = itr->second;
        if (pSession->GetId() != id)
            pSession->SendPacket(packet);
    }
}

void Channel::SendToOne(Packet* packet, uint32 id)
{
    if (Session* ses = FindSession(id))
        ses->SendPacket(packet);
}

void Channel::MakeChannelChangeName(Packet* /*packet*/)
{
    // Settaggio pacchetto
}
