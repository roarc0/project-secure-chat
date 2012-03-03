#include "user-session.h"

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

UserSession::UserSession(SocketServer* Socket, Session* pSes) 
{ 
    m_Socket = Socket;
    m_pSes = pSes;
    MutexInit();
    ResetTime();
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

void UserSession::releaselock_net() 
{
    if (m_pSes)
    {
        m_pSes->getlock_session();
        m_pSes->releaselock_net();
        m_pSes->releaselock_session();
    }
}

void UserSession::releaselock_exec()
{
    if (m_pSes)
    {
        m_pSes->getlock_session();
        m_pSes->releaselock_exec();
        m_pSes->releaselock_session();
    }
}

