#include "channel-manager.h"

// ChannelManagerException Code
ChannelManagerException::ChannelManagerException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) 
{
	if (inclSysMsg) 
	{
		userMessage.append(": ");
		userMessage.append(strerror(errno));
	}
}

ChannelManagerException::~ChannelManagerException() throw() 
{
}

const char *ChannelManagerException::what() const throw() 
{
	return userMessage.c_str();
}


ChannelManager::ChannelManager()
{    
    i_timer.SetInterval(50); //TODO dal config
}

ChannelManager::~ChannelManager()
{
    if (m_updater.IsActivate())
        m_updater.Wait();

    while (!m_channels.empty())
    {
        delete m_channels.begin()->second;
        m_channels.erase(m_channels.begin());
    }
}

Channel* ChannelManager::CreateChannel(std::string& c_name)
{
    Lock guard(m_mutex);

    Channel* m = FindChannel(c_name);
    if (m == NULL)
    {        
        m = new Channel(c_name);
        m_channels[c_name] = m;
    }

    return m;
}

Channel* ChannelManager::FindChannel(std::string& c_name) const
{
    mapChannel::const_iterator iter = m_channels.find(c_name);
    return (iter == m_channels.end() ? NULL : iter->second);
}

int ChannelManager::RemoveChannel(std::string& c_name)
{
    Lock guard(m_mutex);

    mapChannel::iterator iter = m_channels.find(c_name);
    if (iter != m_channels.end())
    {
        delete (iter->second);
        m_channels.erase(iter);
    }

    return 0;
}

bool CanSessionEnter(Session* ses, std::string& c_name) const
{
    Channel* m = FindChannel(c_name);
    if (m)
        if (m->CanSessionEnter(ses))
            return true;
    return false;
}

void ChannelManager::Update(uint32 diff)
{
    i_timer.Update(diff);
    if (!i_timer.Passed())
        return;

    // Gestione della parte ThreadSafe dei Canali
    mapChannel::iterator iter = m_channels.begin();
    for (; iter != m_channels.end(); ++iter)
    {
        if (m_updater.IsActivate())
            m_updater.ScheduleUpdate(*iter->second, uint32(i_timer.GetCurrent()));
        else
            iter->second->Update(uint32(i_timer.GetCurrent()));
    }
    if (m_updater.IsActivate())
        m_updater.Wait();

    // Gestione della parte ThreadUnsafe dei Canali
    for (iter = m_channels.begin(); iter != m_channels.end(); ++iter)
        if (!iter->second->DelayedUpdate(uint32(i_timer.GetCurrent())))
            m_channels.erase(iter);

    i_timer.SetCurrent(0);
}
