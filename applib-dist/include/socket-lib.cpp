#include "socket-lib.hpp"
#include <iostream>
using namespace pipedat;

#pragma region Connection Functions

Connection::Connection(const std::string &ip_address, const unsigned &port)
{
	Connection::Connection(ip_address, port, SocketType::STREAM, Protocol::IPPROTO_TCP);
}

Connection::Connection(const std::string &ip_address, const unsigned &port, const SocketType &type, const Protocol &proto)
{
	// Make sure Windows has started network services
	WSADATA wsaData;
	int startup_result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Check the startup_result
	if (startup_result != 0)
	{
		throw connection_exception("WSAStartup failed: " + startup_result);
		return;
	}

	// Create the socket
	con_socket = socket(AF_INET, type, proto);

	// Create the server address
	sockaddr_in serverAddress = { 0 };
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(ip_address.c_str());

	int res = bind(con_socket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(sockaddr_in));

	if (res == SOCKET_ERROR)
	{
		throw socket_exception("Error on socket bind: " + WSAGetLastError());
		closesocket(con_socket);
		WSACleanup();
	}
}

Connection::Connection(SOCKET sock)
{
	con_socket = sock;
}

void Connection::send(std::string message)
{

}

std::string Connection::receive()
{
	return "";
}

#pragma endregion

#pragma region ConnectionListener Functions

ConnectionListener::ConnectionListener(const unsigned &port, const SocketType &type, const Protocol &proto)
{
	// Make sure Windows has started network services
	WSADATA lpWSAData;
	int startup_result = WSAStartup(MAKEWORD(2, 2), &lpWSAData);

	// Check the startup_result
	if (startup_result != 0)
	{
		throw connection_exception("WSAStartup failed: " + startup_result);
		return;
	}

	listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listening_socket == INVALID_SOCKET)
	{
		std::cout << "Server failed to start. Reason: " << WSAGetLastError() << std::endl;
		return;
	}

	sockaddr_in name;
	memset(&name, 0, sizeof(sockaddr_in));
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.S_un.S_addr = 0; // open port on all network interfaces

								   // Associate our port information with our port
	bind(listening_socket, reinterpret_cast<sockaddr*>(&name), sizeof(sockaddr_in));

	// Open the port for clients to connect, maintaining a backlog of up to 3 waiting connections
	int listen_result = listen(listening_socket, 3);
}

Connection ConnectionListener::wait_for_connection()
{
	// Initialize the client information that will come in once a user joins the server
	sockaddr_in client_information;
	memset(&client_information, 0, sizeof(sockaddr_in));

	// Get the new socket that the user has joined on
	SOCKET client_ID = accept(listening_socket, (sockaddr*)&client_information, NULL);

	// Return the newly generated connection
	return Connection(client_ID);
}

#pragma endregion