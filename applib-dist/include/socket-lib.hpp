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
#include <memory>

namespace pipedat
{
	// a thin wrapper around WSAStartup and WSACleanup to ensure each is called one time
	class WSA_Wrapper
	{
	private:
		WSA_Wrapper();

	public:
		static void startup()
		{
			static WSA_Wrapper wrapper;
		}
		~WSA_Wrapper();
	};

	// Encapsulate the windows socket types inside our own class
	enum SocketType
	{
		STREAM = SOCK_STREAM,
		DGRAM = SOCK_DGRAM,
		RAW = SOCK_RAW,
		RDM = SOCK_RDM,
		SEQPACKET = SOCK_SEQPACKET
	};

	// Encapsulate the windows IPPROTO inside our library
	using Protocol = IPPROTO;

	// Encapsulate the windows SOCKET inside our library
	using ConnectionID = SOCKET;

	class Connection;

	using connection_ptr = std::shared_ptr<Connection>;

	// This object is used to store a connection between two computers
	class Connection
	{
		friend class ConnectionListener;
		friend class std::shared_ptr<Connection>;
		friend class std::_Ref_count_obj<Connection>;

	private:
		SOCKET con_socket;
		Connection(SOCKET sock);

		// delete copy-constructors
		Connection(const Connection &) = delete;
		Connection & operator=(const Connection &) = delete;

	public:
		ConnectionID get_id() const { return con_socket; }

		void send(const std::string & message) const;
		std::string receive() const;
		void shut_down() const;

		Connection(const std::string &ip_address, const unsigned &port);
		Connection(const std::string &ip_address, const unsigned &port, const SocketType &type, const Protocol &proto);

		~Connection() { closesocket(con_socket); } // Upon deconstruction of this object, close the connection's socket
	};

	// This object is used to initialize listening on a port and waiting for a user to connect.
	class ConnectionListener
	{
	private:
		bool socket_closed = false;
		SOCKET listening_socket;

	public:
		ConnectionListener(const unsigned &port, const SocketType &type, const Protocol &proto);
		ConnectionListener(const unsigned &port);
		~ConnectionListener() { if (!socket_closed) { closesocket(listening_socket); } }
		connection_ptr wait_for_connection() const;
		void shut_down();
	};

}

#endif // GUARD_SOCKLIB_HEADER