#ifndef _CHANNEL_H
#define _CHANNEL_H

#include "../../common.h"

class ChannelException : public exception 
{
	public:
		ChannelException(const std::string &message, bool inclSysMsg = false) throw();
		~ChannelException() throw();

		const char *what() const throw();

	private:
		std::string userMessage;  // Exception message
};

class Channel
{
    public:
        Channel(std::string& c_name);
        ~Channel();
    private:
};

#endif
