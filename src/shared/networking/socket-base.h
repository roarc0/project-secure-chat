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

#ifndef SOCKET_BASE_H
#define SOCKET_BASE_H

#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
#include <string>

#include "../common.h"

using namespace std;

#define MAX_QUEUE_CONNECTIONS 16
#define INVALID_SOCKET 0

NEWEXCEPTION(SocketException);

static void fillAddr(const string &address, unsigned short port, 
                     sockaddr_in &addr);


class SocketBase
{
    public:
        virtual ~SocketBase();
        SocketBase(int type, int protocol) throw(SocketException);

        string           getLocalAddress()
                         throw(SocketException);
        unsigned short   getLocalPort()
                         throw(SocketException);

        void             setLocalPort(unsigned short localPort)
                         throw(SocketException);
        void             setLocalAddressAndPort(const string &localAddress, 
                                                unsigned short localPort = 0)
                         throw(SocketException);
        string           getForeignAddress()
                         throw(SocketException);
        unsigned short   getForeignPort()
                         throw(SocketException);

        static unsigned short resolveService(const string &service,
                                             const string &protocol = "tcp");

        void             initSocket()
                         throw(SocketException);

        void             send(const void *buffer, int bufferLen)
                         throw(SocketException);
        int              recv(void *buffer, int bufferLen)
                         throw(SocketException);

    protected:
        int sockDesc;
        int domain, type, protocol;
};

#endif
