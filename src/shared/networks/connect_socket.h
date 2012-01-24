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

#ifndef _CONNECT_SOCKET_H
#define _CONNECT_SOCKET_H

#include "Socket.h"

class ConnectSocket : private Socket
{
	public:

    ConnectSocket ( std::string host, int port );
    virtual ~ClientSocket(){};

	// To change with ByteBuffer
	const ConnectSocket& operator << ( const std::string& ) const;
	const ConnectSocket& operator >> ( std::string& ) const;
};



#endif  /* _CONNECT_SOCKET_H */