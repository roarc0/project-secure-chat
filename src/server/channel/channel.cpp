#include "channel.h"

// ChannelManagerException Code
ChannelException::ChannelException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) 
{
	if (inclSysMsg) 
	{
		userMessage.append(": ");
		userMessage.append(strerror(errno));
	}
}

ChannelException::~ChannelException() throw() 
{
}

const char *ChannelException::what() const throw() 
{
	return userMessage.c_str();
}

Channel::Channel(std::string& c_name) : name(c_name), b_todelete(false)
{

}

Channel::~Channel()
{

}

Session* Channel::FindSession(Session* ses)
{
    mapSession::const_iterator iter = m_sessions.find(ses->GetId());
    return (iter == m_sessions.end() ? NULL : iter->second);
}

Session* Channel::FindSession(uint32 id)
{
    mapSession::const_iterator iter = m_sessions.find(id);
    return (iter == m_sessions.end() ? NULL : iter->second);
}

int Channel::AddSession(Session* ses)
{
    Lock guard(m_mutex);

    for (mapSession::iterator itr = m_sessions.begin(); itr != mapSession.end(); ++itr)
    {
        Session* pSession = *itr;
        Packet new_packet;
        // TODO settaggio pacchetto di aggiornamento nome canale
        pSession->SendPacket(&new_packet);
    }
    return 0;
}


int Channel::SetName(std::string& c_name)
{
    Lock guard(m_mutex);

    name = c_name;

    for (mapSession::iterator itr = m_sessions.begin(); itr != mapSession.end(); ++itr)
    {
        Session* pSession = *itr;
        Packet new_packet;
        // TODO settaggio pacchetto di aggiornamento nome canale
        pSession->SendPacket(&new_packet);
    }
    return 0;
}

void Channel::Update(uint32 t_diff)
{
    //Update Sessions
    for (mapSession::iterator itr = m_sessions.begin(); itr != mapSession.end(); ++itr)
    {
        Session* pSession = *itr;
        MapSessionFilter updater(pSession);
        pSession->Update(t_diff, updater);
    }
}

bool Channel::DelayedUpdate(uint32 t_diff);
{
    return b_todelete ? false : true;
}
