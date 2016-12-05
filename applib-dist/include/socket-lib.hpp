#define _WINSOCK_DEPRECATED_NO_WARNINGS

#if !defined(GUARD_SOCKLIB_HEADER)
#define GUARD_SOCKLIB_HEADER

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

	// This object is used to store a connection between two computers
	class Connection
	{
		friend ConnectionListener;

	private:
		SOCKET con_socket;
		Connection(const unsigned &port, const SocketType &type, const Protocol &proto);

	public:
		void send(std::string message);
		std::string receive();

		Connection(const std::string &ip_address, const unsigned &port);
		Connection(const std::string &ip_address, const unsigned &port, const SocketType &type, const Protocol &proto);
	};

	// This object is used to create a connection object for a server
	class ConnectionListener
	{
	public:
		ConnectionListener(const unsigned &port, const SocketType &type, const Protocol &proto);
		Connection wait_for_connection();
	};

}

#endif // GUARD_SOCKLIB_HEADER