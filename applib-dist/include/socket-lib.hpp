#if !defined(GUARD_SOCKLIB_HEADER)
#define GUARD_SOCKLIB_HEADER
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#if defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "socklib-mt-gd.lib")
#elif defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "socklib-mt-sgd.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "socklib-mt.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "socklib-mt-s.lib")
#endif

#include <WinSock2.h>
#include <Windows.h>
#pragma comment (lib, "Ws2_32.lib")

#include "exception.hpp"

#include <string>

namespace pipedat
{

	// Encapsulate the windows socket types inside our own class
	enum SocketType
	{
		STREAM =	SOCK_STREAM,
		DGRAM =		SOCK_DGRAM,
		RAW =		SOCK_RAW,
		RDM =		SOCK_RDM,
		SEQPACKET = SOCK_SEQPACKET
	};

	// Encapsulate the windows IPPROTO inside our library
	using Protocol = IPPROTO;

	// Encapsulate the windows SOCKET inside our library
	using ConnectionID = SOCKET;

	// This object is used to store a connection between two computers
	class Connection
	{
		friend class ConnectionListener;

	private:
		SOCKET con_socket;
		Connection(SOCKET sock, std::string client_termination_string);
		std::string termination_string;

	public:
		ConnectionID connection_id;

		void send(std::string message) const;
		std::string receive() const;

		Connection(const std::string &ip_address, const unsigned &port);
		Connection(const std::string &ip_address, const unsigned &port, const SocketType &type, const Protocol &proto);

		~Connection() { closesocket(con_socket); } // Upon deconstruction of this object, close the connection's socket
	};

	// This object is used to initialize listening on a port and waiting for a user to connect.
	class ConnectionListener
	{
	private:
		SOCKET listening_socket;
		std::string connection_termination_string;

	public:
		ConnectionListener(const unsigned &port, const SocketType &type, const Protocol &proto, const std::string &termination_string = "");
		ConnectionListener(const unsigned &port, const std::string &termination_string = "");
		Connection wait_for_connection();
		void set_client_termination_string(std::string terminator);
	};

}

#endif // GUARD_SOCKLIB_HEADER