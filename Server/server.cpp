#include "constants.h"
#include "server.h"

#include <sstream>
#include <iostream>
#include <iterator>

using namespace pipedat;

Server::Server(const unsigned &height, const unsigned &width)
{
	// configure the window
	Console_Framework::setup(height, width, "Server");
	Console_Framework::set_cursor_position(0, 0);
	Console_Framework::set_cursor_visibility(false);
}

void Server::run()
{
	// start the listening thread
	std::thread new_users_thread(&Server::listen_for_new_users, this);

	// start the receiving thread
	std::thread send_thread(&Server::send, this);

	// start the handling of threads
	std::thread events_thread(&Server::handle_events, this);

	try
	{
		for (;;)
		{
			// get the next message
			const Message message = input_queue.get();

			if (message.data.size() == 0)
				continue;

			// check if the user has entered a special command
			if (message.data[0] == '/')
			{
				std::stringstream ss(message.data);
				const std::istream_iterator<std::string> begin(ss);
				std::vector<std::string> strings(begin, std::istream_iterator<std::string>());

				if (strings.size() > 0)
				{
					std::lock_guard<std::mutex> lock(users_mutex);
					handle_commands(users[message.id]->connection, strings);
				}

				continue;
			}

			// prepend the username to the message and retrieve the user's room
			std::string data;
			std::string room_name;
			{
				std::lock_guard<std::mutex> lock(users_mutex);
				data = users[message.id]->user_name + ": " + message.data;
				room_name = users[message.id]->room_name;
			}

			// forward their message to the room
			send_to_room(room_name, data, message.id);
		}
	}
	catch (threadsafe::queue<Message>::queue_quit)
	{
	}

	// terminate each connection
	{
		std::lock_guard<std::mutex> lock(users_mutex);
		for (auto & user : users)
			user.second->connection->shut_down();
	}

	// wait for the threads to exit
	new_users_thread.join();
	send_thread.join();
	events_thread.join();

}

void Server::handle_events()
{
	for (;;)
	{
		// get the list of events that have occurred
		const Console_Framework::event_list events = Console_Framework::get_events();

		// for each event
		for (const Console_Framework::event_ptr & event : events)
		{
			// if the event is a done event type
			if (const Console_Framework::done_event_ptr done_event = Console_Framework::convert_to<Console_Framework::Done_Event>(event))
			{
				finished = true;

				connection_listener->shut_down();

				input_queue.quit();
				output_queue.quit();

				// terminate each connection
				{
					std::lock_guard<std::mutex> lock(users_mutex);
					for (auto & user : users)
						user.second->connection->shut_down();
				}

				return; // Destroy this thread as it no longer needed
			}
		}
	}
}

void Server::listen_for_new_users()
{
	connection_listener = std::make_unique<ConnectionListener>(8050, SocketType::STREAM, Protocol::IPPROTO_TCP);

	std::cout << "Listening for new users on port 8050.\n";

	for (;;)
	{
		// get the next new connection
		connection_ptr connection = connection_listener->wait_for_connection();

		// kill this thread if the connection listener fails
		if (connection == nullptr)
		{
			// if the server is closing down, wait for child threads to detect it and close
			//if (finished)
			//{
			//	std::lock_guard<std::mutex> lock(users_mutex);
			//	for (auto & user : users)
			//		user.second->thread->join();
			//}

			return;
		}

		// build the username using a stringstreawm
		std::stringstream ss;
		ss << "User" << connection->get_id();

		// add the user to the list of users
		{
			std::lock_guard<std::mutex> lock(users_mutex);
			users[connection->get_id()] = std::make_unique<User_Info>(connection, ss.str(), "main");
		}

		// add the user to the main room
		{
			std::lock_guard<std::mutex> lock(room_mutex);
			if (rooms.find("main") == rooms.cend()) // if the main room hasn't been created yet
				rooms["main"] = std::set<pipedat::ConnectionID>();
			rooms["main"].insert(connection->get_id());
		}

		// Notify the other users in the room
		{
			std::lock_guard<std::mutex> lock(users_mutex);
			send_to_room("main", (C::INFO_FLAG + users[connection->get_id()]->user_name + " has joined the room"), connection->get_id());
		}

		// start the user's receive thread
		std::thread(&Server::receive, this, connection).detach();
	}

}

void Server::receive(const connection_ptr connection)
{
	for (;;)
	{
		std::string data;

		try
		{
			data = connection->receive();
		}
		catch (pipedat::disgraceful_disconnect_exception) {}
		catch (std::exception) {}

		if (finished) return;

		// in the event of a disconnect or failure, an empty message signals the server to clean up after the user
		if (data.size() == 0)
		{
			remove_user(connection);
			return;
		}
		else
		{
			// save the user's message
			input_queue.put(Message(connection->get_id(), data));
		}
	}
}

void Server::send()
{
	try
	{
		for (;;)
		{
			// get the next message on the queue
			const Message message = output_queue.get();
			// lock the user mutex
			std::lock_guard<std::mutex> lock(users_mutex);
			// send the message
			users[message.id]->connection->send(message.data);
		}
	}
	catch (threadsafe::queue<Message>::queue_quit)
	{
		// the thread now dies
	}
}

void Server::handle_commands(const connection_ptr connection, const std::vector<std::string> & commands)
{
	const std::string command = commands[0];

	// the caller locks the users_mutex
	const auto user_it = users.find(connection->get_id());

	if (command == "/name" || command == "/n")
	{
		if (commands.size() < 2)
		{
			return;
		}
		else if (commands.size() > 2)
		{
			send_to_user(connection->get_id(), (C::INFO_FLAG + "You cannot have spaces in your username."));
			return;
		}

		for (const auto & user : users)
		{
			if (user.second->user_name == commands[1])
			{
				send_to_user(connection->get_id(), (C::INFO_FLAG + commands[1] + " is already in use."));
				return;
			}
		}

		// tell the room that the user has changed their name
		send_to_room(user_it->second->room_name, (C::INFO_FLAG + user_it->second->user_name + " has changed their name to " + commands[1]), user_it->second->connection->get_id());

		send_to_user(user_it->second->connection->get_id(), (C::INFO_FLAG + "You have changed your name to " + commands[1]));

		// Change the user's name
		user_it->second->user_name = commands[1];
	}
	else if (command == "/join" || command == "/j")
	{
		if (commands.size() < 2)
		{
			return;
		}
		else if (commands.size() > 2)
		{
			send_to_user(connection->get_id(), (C::INFO_FLAG + "You cannot have spaces in your room names."));
			return;
		}

		const std::string new_room_name = commands[1];

		// Tell the other users that this user has left the room
		send_to_room(user_it->second->room_name, (C::INFO_FLAG + user_it->second->user_name + " has left the room."), user_it->second->connection->get_id());

		// Remove the user from the current room and add them to the new room
		{
			// Lock both user and room mutex since we will be modifying both
			std::lock_guard<std::mutex> room_lock(room_mutex);

			// Get the user and room iterators
			auto room_it = rooms.find(user_it->second->room_name);

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
		user_it->second->room_name = new_room_name;

		// Tell the other users that this user has joined the room
		send_to_room(user_it->second->room_name, (C::INFO_FLAG + user_it->second->user_name + " has joined the room."), user_it->second->connection->get_id());

		// Tell the user that they have joined the room. We can't do this client-side, because a client
		// has no guarantees of the functionality of the server.
		send_to_user(user_it->first, (C::INFO_FLAG + "You have joined " + new_room_name + "."));
	}
	else if (command == "/whisper" || command == "/w")
	{
		if (commands.size() > 1)
		{
			// Get the username of the receiver
			std::string receiver = commands[1];

			// Lock both room mutex as we are looking for users in the current room
			std::lock_guard<std::mutex> room_lock(room_mutex);

			bool user_exists = false;
			for (auto & user : users)
			{
				if (user.second->user_name == receiver)
				{
					std::string message;
					for (unsigned i = 2; i < commands.size(); ++i)
						message += commands[i] + " ";

					// Remove the last space at the end of the message
					message = message.substr(0, message.size() - 1);

					send_to_user(user.second->connection->get_id(), (C::WHISPER_FLAG + user_it->second->user_name + ": " + message));

					user_exists = true;
					break;
				}
			}

			if (!user_exists)
				send_to_user(user_it->first, (C::INFO_FLAG + "A user with username: " + receiver + " is not online."));
		}
		else
			send_to_user(user_it->first, (C::INFO_FLAG + "You must specify a user to whisper to."));
	}
	else if (command == "/help" || command == "/h")
	{
		std::string help_message = C::HELP_FLAG + "Use one of the following commands:";
		help_message += C::HELP_FLAG + "up arrow key - Used to scroll up through previous messages in the chat room.";
		help_message += C::HELP_FLAG + "down arrow key - Used to scroll down through messages in the chat room.";
		help_message += C::HELP_FLAG + "/name | /n - Used in conjunction with a new name to change your name.";
		help_message += C::HELP_FLAG + "/join | /j - Used in conjunction with a room name to join a new room.";
		help_message += C::HELP_FLAG + "/whisper | /w - Use this command in conjunction with the name of the person you'd like to whisper and a message.";
		help_message += C::HELP_FLAG + "/exit | /e - Used to exit the application.";

		send_to_user(user_it->first, help_message);
	}
	else
	{
		send_to_user(user_it->first, (C::INFO_FLAG + commands[0] + "is not a recognized command. Use /help to view a list of commands."));
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

void Server::remove_user(const connection_ptr connection)
{
	// Lock both user and room mutex since we will be modifying both
	std::lock_guard<std::mutex> user_lock(users_mutex);

	// Get the user and room iterators
	const auto user_it = users.find(connection->get_id());

	// Tell the other users that this user has left the room
	send_to_room(user_it->second->room_name, (C::INFO_FLAG + user_it->second->user_name + " has left the room."), user_it->second->connection->get_id());

	// Lock the rooms mutex as we need to remove the user from this room
	std::lock_guard<std::mutex> room_lock(room_mutex);
	const auto room_it = rooms.find(user_it->second->room_name);

	// Erase the user from the room
	room_it->second.erase((room_it->second.find(connection->get_id())));

	// If the room has no one left in it, destroy that room
	if (room_it->second.size() == 0)
		rooms.erase(room_it);

	// Since the user has left the server, remove them from the server's list of users
	users.erase(user_it);
}
