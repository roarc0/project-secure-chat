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

#ifndef _SERVER_SOCKET_H
#define _SERVER_SOCKET_H

#include "Socket.h"

class ServerSocket : private Socket
{
	public:
	
		ServerSocket ( int port );
		ServerSocket (){};
		virtual ~ServerSocket();

		// To change with con ByteBuffer
		const ServerSocket& operator << ( const std::string& ) const;
		const ServerSocket& operator >> ( std::string& ) const;

		void accept ( ServerSocket& );
};



#endif  /* _SERVER_SOCKET_H */