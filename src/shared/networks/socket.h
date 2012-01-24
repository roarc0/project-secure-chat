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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Packet;

class SocketException
{
	public:
		SocketException (std::string s) { m_s = s; };
		~SocketException (){};

		std::string description() { return m_s; }

	private:

		std::string m_s;
};

class Socket
{
    public:
        Socket ();
        ~Socket ();
		
		/// Mutex type used for various synchronizations.
		/// TODO
		
		void close_socket ();

	    /// Server initialization
	    bool create ();
	    bool bind (const int port);
	    bool listen () const;
	    bool accept (Socket&) const;

	    /// Client initialization
	    bool connect (const std::string host, const int port);
		
		// Temp send with String
		bool send ( const std::string s ) const
		
        /// Send A packet on the socket, this function is reentrant.
        /// @param pct packet to send
        /// @return -1 of failure
        //int send_packet (const Packet& pct);
		
		// Temp recv with String
		int recv ( std::string& s ) const
		
		/// @return -1 of failure
		//int recv_packet (const Packet& pct);
		
		void set_non_blocking (const bool);

        bool is_valid() const { return m_sock != -1; }
	
	private:
	
	    int m_sock;
		
        sockaddr_in m_addr;
	
	    /// Mutex private

		/// Class used for managing encryption
        //Crypt m_Crypt;
		
		/// process one incoming packet.
        /// @param new_pct received packet
        //int process_incoming (Packet* new_pct);
		
		/// Called by ProcessIncoming()
        //int handle_packet (Packet& recvPacket);

        /// Called by ProcessIncoming() on CMSG_PING.
        //int handle_ping (Packet& recvPacket);
		
		/// Helper functions for processing incoming data.
        //int handle_input_header ();
        //int handle_input_payload ();
        //int handle_input_missing_data ();
}


#endif  /* _SOCKET_H */