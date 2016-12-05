
#include "server.h"

using namespace pipedat;

Server::Server()
{

}

void Server::run()
{
	// start the listening thread
	std::thread(&Server::listen_for_new_users, this).detach();

	// start the receiving thread
	std::thread(&Server::send, this).detach();

	for (;;)
	{
		// get the next message
		const Message message = input_queue.get();

		// get the user's room
		std::string room_id;
		{
			std::lock_guard<std::mutex> lock(users_mutex);
			room_id = users[message.id].second;
		}

		// get the users in the room
		std::set<pipedat::ConnectionID> users_in_room;
		{
			std::lock_guard<std::mutex> lock(room_mutex);
			users_in_room = rooms[room_id];
		}

		// for each user in the room
		for (const auto & user : users_in_room)
			// if the user is not the sender
			if (user != message.id)
				// send a new message
				output_queue.put(Message(user, message.data));
	}
}

void Server::listen_for_new_users()
{
	pipedat::ConnectionListener connection_listener(8050, SocketType::STREAM, Protocol::IPPROTO_TCP);

	for (;;)
	{
		// get the next new connection
		connection_ptr connection = connection_listener.wait_for_connection();

		// save the user
		{
			std::lock_guard<std::mutex> lock(users_mutex);
			users[connection->get_id()] = make_pair(connection, "main");
		}

		{
			std::lock_guard<std::mutex> lock(room_mutex);

			// make sure the main room exists
			if (rooms.find("main") == rooms.cend())
				rooms["main"] = std::set<pipedat::ConnectionID>();

			// add the user to the main room
			rooms["main"].insert(connection->get_id());
		}

		// start a thread to listen to this user
		std::thread(&Server::receive, this, connection).detach();
	}

}

void Server::receive(connection_ptr connection)
{
	for (;;)
	{
		const std::string data = connection->receive();

		if (data.size() == 0) return;

		input_queue.put(Message(connection->get_id(), data));
	}
}

void Server::send()
{
	for (;;)
	{
		// get the next message on the queue
		const Message message = output_queue.get();
		// send the message
		users[message.id].first->send(message.data);
	}
}
