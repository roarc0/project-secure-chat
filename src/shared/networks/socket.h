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

class Packet;

class SocketException
{
	public:
		SocketException (std::string s) { m_s = s; };
		~SocketException (void){};

		std::string description() { return m_s; }

	private:

		std::string m_s;
};

class Socket
{
    public:
        Socket (void);
        ~Socket (void);
		
		/// Mutex type used for various synchronizations.
		/// TODO

	    /// Server initialization
	    bool create (void);
	    bool bind (const int port);
	    bool listen (void) const;
	    bool accept (Socket&) const;

	    /// Client initialization
	    bool connect (const std::string host, const int port);
		
        /// Send A packet on the socket, this function is reentrant.
        /// @param pct packet to send
        /// @return -1 of failure
        int send_packet (const Packet& pct);
		
		/// @return -1 of failure
		int get_packet (const Packet& pct);
		
		void set_non_blocking (const bool);

        bool is_valid() const { return m_sock != -1; }
		
		void close_socket (void);
	
	private:
	
	    int m_sock;
		
        sockaddr_in m_addr;
	
	    /// Mutex private

		/// Class used for managing encryption
        Crypt m_Crypt;
		
		/// process one incoming packet.
        /// @param new_pct received packet
        int process_incoming (Packet* new_pct);
		
		/// Called by ProcessIncoming()
        int handle_packet (Packet& recvPacket);

        /// Called by ProcessIncoming() on CMSG_PING.
        int handle_ping (Packet& recvPacket);
		
		/// Helper functions for processing incoming data.
        int handle_input_header (void);
        int handle_input_payload (void);
        int handle_input_missing_data (void);
}


#endif  /* _SOCKET_H */