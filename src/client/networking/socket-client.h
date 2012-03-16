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

#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include "../../shared/networking/socket-base.h"

using namespace std;

#define MAX_QUEUE_CONNECTIONS 16
#define INVALID_SOCKET 0

class SocketClient : public SocketBase
{
    public:
        ~SocketClient();
        SocketClient(int type, int protocol) : SocketBase() throw(SocketException);

        void             Connect(const string &foreignAddress,
                                 unsigned short foreignPort)
                         throw(SocketException);
        void             Disconnect()
                         throw(SocketException);
};

#endif
