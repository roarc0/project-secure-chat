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

#ifndef _CLIENT_SOCKET_H
#define _CLIENT_SOCKET_H

#include "Socket.h"

class ClientSocket : private Socket
{
	public:

		ClientSocket ( std::string host, int port );
		virtual ~ClientSocket(){};
		
		connect();

		// To change with ByteBuffer
		const ClientSocket& operator << ( const std::string& ) const;
		const ClientSocket& operator >> ( std::string& ) const;
		
	private:
	
		std::string m_host;
		int m_port;
};
#endif  /* _Client_SOCKET_H */