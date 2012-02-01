#include "session.h"

// SocketException Code
UserSessionException::UserSessionException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) 
{
	if (inclSysMsg) 
	{
		userMessage.append(": ");
		userMessage.append(strerror(errno));
	}
}

UserSessionException::~UserSessionException() throw() 
{
}

const char *UserSessionException::what() const throw() 
{
	return userMessage.c_str();
}

UserSession::UserSession(uint32 id, TCPSocket* Socket) 
{ 
    m_id = id; 
    m_Socket = Socket; 
    MutexInit();
}

void UserSession::QueuePacketToRecv(Packet* new_packet)
{
    _recvQueue.push_back(new_packet);
}

void UserSession::QueuePacketToSend(Packet* new_packet)
{
    _sendQueue.push_back(new_packet);
}

Packet* UserSession::GetPacketFromRecv()
{
    list<Packet*>::iterator itr = _recvQueue.begin();

}

Packet* UserSession::GetPacketFromSend()
{

}

