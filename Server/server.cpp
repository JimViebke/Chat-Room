
#include "server.h"

#include <sstream>
#include <iterator>

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
		Message message = input_queue.get();

		// check if the user has entered a special command
		if (message.data[0] == '/')
		{
			std::stringstream ss(message.data);
			const std::istream_iterator<std::string> begin(ss);
			std::vector<std::string> strings(begin, std::istream_iterator<std::string>());

			if (strings.size() > 0)
			{
				std::lock_guard<std::mutex> lock(users_mutex);
				handle_commands(users[message.id].connection, strings);
			}

			continue;
		}

		// get the user's room
		std::string room_name;
		{
			std::lock_guard<std::mutex> lock(users_mutex);
			room_name = users[message.id].room_name;
		}

		// forward their message to the room
		send_to_room(room_name, message.data, message.id);
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
			users[connection->get_id()] = User_Info(connection, "User " + connection->get_id(), "main");
		}

		// add the user to the main room
		{
			std::lock_guard<std::mutex> lock(room_mutex);
			if (rooms.find("main") == rooms.cend()) // if the main room hasn't been created yet
				rooms["main"] = std::set<pipedat::ConnectionID>();
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

		if (data.size() == 0) return; // 0 indicates connection termination

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
		users[message.id].connection->send(message.data);
	}
}

void Server::handle_commands(connection_ptr connection, const std::vector<std::string> & commands)
{
	// the caller locks the users_mutex

	auto user_it = users[connection->get_id()];

	if (commands[0] == "/name")
	{
		if (commands.size() < 2) return;

		for (const auto & user : users)
		{
			if (user.second.user_name == commands[1])
			{
				output_queue.put(Message(connection->get_id(), commands[1] + " is already in use."));
				return;
			}
		}

		// tell the room that the user has changed their name
		send_to_room(user_it.room_name, user_it.user_name + " has changed their name to " + commands[1], user_it.connection->get_id());

		// change the user's name
		user_it.user_name = commands[1];
	}
	else if (commands[0] == "/join")
	{

	}
	else if (commands[0] == "/exit")
	{

	}
	else if (commands[0] == "/help")
	{

	}
}

void Server::send_to_room(const std::string & room_name, const std::string & data, const ConnectionID & exclude)
{
	// get the users in the room
	std::set<pipedat::ConnectionID> users_in_room;
	{
		std::lock_guard<std::mutex> lock(room_mutex);
		users_in_room = rooms[room_name];
	}

	// for each user in the room
	for (const ConnectionID & user : users_in_room)
		// if the user is not the sender
		if (user != exclude)
			// send a new message
			output_queue.put(Message(user, data));
}