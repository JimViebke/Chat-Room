
#include "server.h"

#include <sstream>
#include <iostream>
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
		const Message message = input_queue.get();

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
		
		// prepend the username to the message and retrieve the user's room
		std::string data;
		std::string room_name;
		{
			std::lock_guard<std::mutex> lock(users_mutex);
			data = users[message.id].user_name + ": " + message.data;
			room_name = users[message.id].room_name;
		}

		// forward their message to the room
		send_to_room(room_name, data, message.id);
	}
}

void Server::listen_for_new_users()
{
	pipedat::ConnectionListener connection_listener(8050, SocketType::STREAM, Protocol::IPPROTO_TCP);

	for (;;)
	{
		// get the next new connection
		connection_ptr connection = connection_listener.wait_for_connection();

		// build the username using a stringstreawm
		std::stringstream ss;
		ss << "User " << connection->get_id();

		// add the user to the list of users
		{
			std::lock_guard<std::mutex> lock(users_mutex);
			users[connection->get_id()] = User_Info(connection, ss.str(), "main");
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
		std::string data;

		try
		{
			data = connection->receive();
		}
		catch (pipedat::disgraceful_disconnect_exception & dde)
		{
			std::cout << "Caught disgraceful_disconnect_exception on connection->receive(). Error message: [" + std::string(dde.what()) + "].";
		}
		catch (std::exception & ex)
		{
			std::cout << "Caught std::exception connection->receive(). Error message: [" + std::string(ex.what()) + "].";
		}

		// in the event of a disconnect or failure, an empty message signals the server to clean up after the user
		if (data.size() == 0)
		{			
			remove_user(connection);
			return;
		}

		// save the user's message
		input_queue.put(Message(connection->get_id(), data));
	}
}

void Server::send()
{
	for (;;)
	{
		// get the next message on the queue
		const Message message = output_queue.get();
		// lock the user mutex
		std::lock_guard<std::mutex> lock(users_mutex);
		// send the message
		users[message.id].connection->send(message.data);
	}
}

void Server::handle_commands(connection_ptr connection, const std::vector<std::string> & commands)
{
	std::string command = commands[0];

	// the caller locks the users_mutex
	auto user_it = users.find(connection->get_id());

	if (command == "/name")
	{
		std::string new_user_name = commands[1];

		if (commands.size() < 2) return;

		for (const auto & user : users)
		{
			if (user.second.user_name == new_user_name)
			{
				send_to_user(connection->get_id(), new_user_name + " is already in use.");
				return;
			}
		}

		// tell the room that the user has changed their name
		send_to_room(user_it->second.room_name, user_it->second.user_name + " has changed their name to " + new_user_name, user_it->second.connection->get_id());

		send_to_user(user_it->second.connection->get_id(), "You have changed your name to " + new_user_name);

		// Change the user's name
		user_it->second.user_name = new_user_name;
	}
	else if (command == "/join")
	{
		std::string new_room_name = commands[1];

		if (commands.size() < 2) return;

		// Tell the other users that this user has left the room
		send_to_room(user_it->second.room_name, user_it->second.user_name + " has left the room.", user_it->second.connection->get_id());

		// Remove the user from the current room and add them to the new room
		{
			// Lock both user and room mutex since we will be modifying both
			std::lock_guard<std::mutex> room_lock(room_mutex);

			// Get the user and room iterators
			auto room_it = rooms.find(user_it->second.room_name);

			// Erase the user from the room
			room_it->second.erase((room_it->second.find(connection->get_id())));

			// If the room has no one left in it, destroy that room
			if (room_it->second.size() == 0)
				rooms.erase(room_it);

			if (rooms.find(new_room_name) == rooms.cend()) // if the room hasn't been created yet
				rooms[new_room_name] = std::set<pipedat::ConnectionID>();

			// Add the user to the new room
			rooms[new_room_name].insert(connection->get_id());
		}

		// Move this user to the new room
		user_it->second.room_name = new_room_name;
		
		// Tell the other users that this user has left the room
		send_to_room(user_it->second.room_name, user_it->second.user_name + " has joined the room.", user_it->second.connection->get_id());
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
			send_to_user(user, data);
}

void Server::send_to_user(const ConnectionID &user, const std::string &data)
{
	output_queue.put(Message(user, data));
}

void Server::remove_user(connection_ptr connection)
{
	// Lock both user and room mutex since we will be modifying both
	std::lock_guard<std::mutex> user_lock(users_mutex);

	// Get the user and room iterators
	auto user_it = users.find(connection->get_id());

	// Tell the other users that this user has left the room
	send_to_room(user_it->second.room_name, user_it->second.user_name + " has left the room.", user_it->second.connection->get_id());

	// Lock the rooms mutex as we need to remove the user from this room
	std::lock_guard<std::mutex> room_lock(room_mutex);
	auto room_it = rooms.find(user_it->second.room_name);

	// Erase the user from the room
	room_it->second.erase((room_it->second.find(connection->get_id())));

	// If the room has no one left in it, destroy that room
	if (room_it->second.size() == 0)
		rooms.erase(room_it);

	// Since the user has left the server, remove them from the server's list of users
	users.erase(user_it);
}
