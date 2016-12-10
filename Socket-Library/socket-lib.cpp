#include "socket-lib.hpp"
#include <iostream>
#include <sstream>
using namespace pipedat;

WSA_Wrapper::WSA_Wrapper()
{
	WSADATA wsaData;
	const int startup_result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (startup_result != 0)
		throw connection_exception("WSAStartup failed: " + startup_result);
}

WSA_Wrapper::~WSA_Wrapper()
{
	WSACleanup();
}

#pragma region Connection Functions

Connection::Connection(const std::string &ip_address, const unsigned &port) : Connection(ip_address, port, SocketType::STREAM, Protocol::IPPROTO_TCP) { }

Connection::Connection(const std::string &ip_address, const unsigned &port, const SocketType &type, const Protocol &proto)
{
	// ensure WSA has started (destroys automatically)
	WSA_Wrapper::startup();

	// Create the socket
	con_socket = socket(AF_INET, type, proto);

	// Create the server address
	sockaddr_in serverAddress = { 0 };
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());

	const int res = connect(con_socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in));

	if (res == SOCKET_ERROR)
	{
		closesocket(con_socket);
		throw socket_exception("Error on socket connect: " + WSAGetLastError());
	}
}

Connection::Connection(SOCKET sock)
{
	// ensure WSA has started (destroys automatically)
	WSA_Wrapper::startup();

	con_socket = sock;
}

void Connection::send(const std::string & message) const
{
	::send(con_socket, message.c_str(), (int)message.size(), 0);
}

std::string Connection::receive() const
{
	char input[1024];

	for (;;)
	{
		// Execution pauses inside of recv until the user sends data
		int data_read = recv(con_socket, input, 1024, 0);

		// Check for a graceful disconnect OR a less graceful disconnect
		if (data_read == 0 || data_read == -1)
			throw disgraceful_disconnect_exception("data_read is -1");

		std::stringstream ss;
		for (int i = 0; i < data_read; ++i)
			ss << input[i];

		return ss.str();
	}
}

void Connection::shut_down() const
{
	shutdown(con_socket, SD_BOTH);
}

#pragma endregion

#pragma region ConnectionListener Functions

ConnectionListener::ConnectionListener(const unsigned &port) : ConnectionListener(port, SocketType::STREAM, Protocol::IPPROTO_TCP) {}

ConnectionListener::ConnectionListener(const unsigned &port, const SocketType &type, const Protocol &proto)
{
	// ensure WSA has started (destroys automatically)
	WSA_Wrapper::startup();

	listening_socket = socket(AF_INET, type, proto);

	if (listening_socket == INVALID_SOCKET)
		throw listen_exception("Failed to listen on port. Reason: " + WSAGetLastError());

	sockaddr_in name;
	memset(&name, 0, sizeof(sockaddr_in));
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.S_un.S_addr = 0; // Open port on all network interfaces

								   // Associate our port information with our port
	bind(listening_socket, reinterpret_cast<sockaddr*>(&name), sizeof(sockaddr_in));

	// Open the port for clients to connect, maintaining a backlog of up to 3 waiting connections
	int listen_result = listen(listening_socket, 3);
}

connection_ptr ConnectionListener::wait_for_connection() const
{
	// Initialize the client information that will come in once a user joins the server
	sockaddr_in client_information;
	memset(&client_information, 0, sizeof(sockaddr_in));

	// Get the new socket that the user has joined on
	SOCKET client_ID = accept(listening_socket, (sockaddr*)&client_information, NULL);

	if (client_ID == INVALID_SOCKET) return nullptr;

	// Return the newly generated connection
	return std::make_shared<Connection>(client_ID);
}

void ConnectionListener::shut_down()
{
	socket_closed = true;
	closesocket(listening_socket);
}

#pragma endregion