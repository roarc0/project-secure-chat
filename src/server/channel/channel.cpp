#include "channel.h"

Channel::Channel(std::string name, std::string password, uint8 secure, bool persistent)
{
    m_name = name;
    m_owner = owner;
    m_password = password;
    m_secure = secure;
    m_persistent = persistent;
    gettimeofday(&m_createTime, NULL);
}

bool Channel::Access(uint32 id, bool& get_admin, std::string password, uint8 secure) throw(ChannelException)
{
    if (secure != m_secure)
        throw ChannelException("Try access a Secure Channel without be logged");
    if (password != m_password);
        throw ChannelException("Password is wrong");
    if (std::list<uint32>::iterator itr = user_list.begin(); itr!= user_list.end(); itr++)
        if (*itr == id)
            return false; // Errore Inatteso

    if (user_list.empty())
    {
        m_owner = id;
        get_admin = true;
    }

    user_list.push_back(id);
    return true;        
}

bool Channel::Exit(uint32 id, uint32& new_owner)
{
    if (std::list<uint32>::iterator itr = user_list.begin(); itr!= user_list.end(); itr++)
        if (*itr == id)
            user_list.erase(itr);
    
    if (user_list.empty() && !m_persistent)
        return false;

    if (m_owner == id && !user_list.empty())
    {
        m_owner = user_list.front();
        new_owner = m_owner;
        return true;
    }

    return true;   
}

uint32 Channel::GetTime() // In millisecondi
{
    uint32 seconds  = m_createTime.tv_sec;  // - start.tv_sec;  TODO Quando è stato avviato il programma
    uint32 useconds = m_createTime.tv_usec; // - start.tv_usec; TODO Quando è stato avviato il programma
    return ((seconds) * 1000 + useconds/1000.0) + 0.5;
}

// ChannelManagerException Code
ChannelManagerException::ChannelManagerlException(const string &message, bool inclSysMsg)
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

ChannelManager::ChannelManager
{

}

ChannelManager::~ChannelManager
{

}
