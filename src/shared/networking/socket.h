/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SOCKET_H
#define _SOCKET_H

#include <string>
#include <exception>
#include "../common.h"

using namespace std;

#define MAX_QUEUE_CONNECTIONS 5
#define INVALID_SOCKET 0

class SocketException : public exception 
{
    public:
        SocketException(const std::string &message, bool inclSysMsg = false) throw();
        ~SocketException() throw();

        const char *what() const throw();

    private:
        std::string userMessage;  // Exception message
};

class Socket
{
    public:
        ~Socket();

        string getLocalAddress() throw(SocketException);

        unsigned short getLocalPort() throw(SocketException);

        void setLocalPort(unsigned short localPort) throw(SocketException);

        void setLocalAddressAndPort(const string &localAddress, 
        unsigned short localPort = 0) throw(SocketException);

        static unsigned short resolveService(const string &service,
                                           const string &protocol = "tcp");

        void setBlocking(const bool b) throw(SocketException);

    private:
        Socket(const Socket &sock);
        void operator=(const Socket &sock);

    protected:
        int sockDesc;    // Socket descriptor
        fd_set fd_sock;
        bool block;

        Socket(int type, int protocol) throw(SocketException);
        Socket(int sockDesc);
};

class CommunicatingSocket : public Socket 
{
    public:
        void connect(const string &foreignAddress, unsigned short foreignPort)
        throw(SocketException);
        void disconnect() throw(SocketException);

        void send(const void *buffer, int bufferLen) throw(SocketException);
        int recv(void *buffer, int bufferLen) throw(SocketException);



        string getForeignAddress() throw(SocketException);

        unsigned short getForeignPort() throw(SocketException);

    protected:
        CommunicatingSocket(int type, int protocol) throw(SocketException);
        CommunicatingSocket(int newConnSD);
};

class TCPSocket : public CommunicatingSocket 
{
    public:
        TCPSocket() throw(SocketException);

        TCPSocket(const string &foreignAddress, unsigned short foreignPort)
          throw(SocketException);

    private:
        friend class TCPServerSocket;
        TCPSocket(int newConnSD);
};

class TCPServerSocket : public Socket 
{
    public:
        TCPServerSocket(unsigned short localPort, bool block, int queueLen = MAX_QUEUE_CONNECTIONS) 
          throw(SocketException);

        TCPServerSocket(const string &localAddress, unsigned short localPort, 
          bool block, int queueLen = MAX_QUEUE_CONNECTIONS) throw(SocketException);

        TCPSocket *accept() throw(SocketException);

    private:
        void setListen(int queueLen) throw(SocketException);
};

#endif  /* _SOCKET_H */
