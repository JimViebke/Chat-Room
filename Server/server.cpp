
#include "server.h"

using namespace pipedat;

Server::Server()
{

}

void Server::run()
{
	std::thread(&Server::listen_for_new_users, this).detach();
}

void Server::listen_for_new_users()
{
	pipedat::ConnectionListener connection_listener(8050, SocketType::STREAM, Protocol::IPPROTO_TCP);

	for (;;)
	{
		Connection connection = connection_listener.wait_for_connection();

		std::thread(&Server::read_from_connection, this, connection).detach();
	}

}

void Server::read_from_connection(Connection connection)
{
	for (;;)
	{
		const std::string message = connection.receive();
	}
}