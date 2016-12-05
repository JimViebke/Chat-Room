#include "socket-lib.hpp";
#include <iostream>
using namespace pipedat;

#pragma region Connection Functions

Connection::Connection(const std::string &ip_address, const unsigned &port)
{
	Connection::Connection(ip_address, port, SocketType::STREAM, Protocol::IPPROTO_TCP);
}

Connection::Connection(const std::string &ip_address, const unsigned &port, const SocketType &type, const Protocol &proto)
{
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

void Connection::send(std::string message)
{

}

std::string Connection::receive()
{

}

#pragma endregion

#pragma region ConnectionListener Functions

ConnectionListener::ConnectionListener(const unsigned &port, const SocketType &type, const Protocol &proto)
{

}

Connection ConnectionListener::wait_for_connection()
{

}

#pragma endregion