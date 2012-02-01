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
    getlock_recv();
    _recvQueue.push_back(new_packet);
    releaselock_recv();
}

void UserSession::QueuePacketToSend(Packet* new_packet)
{
    getlock_send();
    _sendQueue.push_back(new_packet);
    releaselock_send();
}

int UserSession::RecvSize()
{
    getlock_recv();
    int size = _recvQueue.size();
    releaselock_recv();
}

int UserSession::SendSize()
{
    getlock_send();
    int size = _sendQueue.size();
    releaselock_send();
}

Packet* UserSession::GetPacketFromRecv()
{
    getlock_recv();
    Packet* pck = NULL;
    if (!_recvQueue.empty())
    {
        pck = _recvQueue.front();
        _recvQueue.pop_front();
    }    
    releaselock_recv();
    return pck;
}

Packet* UserSession::GetPacketFromSend()
{
    getlock_send();
    Packet* pck = NULL;
    if (!_sendQueue.empty())
    {
        pck = _sendQueue.front();
        _sendQueue.pop_front();
    }    
    releaselock_send();
    return pck;
}

