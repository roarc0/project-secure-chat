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

Channel::Channel(std::string& name)
{

}

Channel::~Channel()
{

}
