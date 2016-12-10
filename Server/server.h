#pragma once

#include <socket-lib.hpp>

#include <map>
#include <set>
#include <string>
#include <mutex>
#include <atomic>

#include "threadsafe_queue.h"
#include "console_framework.h"

// a simple class to bundle an ID with a string
class Message
{
public:
	pipedat::ConnectionID id;
	std::string data;

	Message(const pipedat::ConnectionID & set_id, const std::string & set_data) : id(set_id), data(set_data) {}
};

class Server
{
private:
	std::atomic<bool> finished = false;
	using connection_ptr = std::shared_ptr<pipedat::Connection>;

	std::unique_ptr<pipedat::ConnectionListener> connection_listener;

	class User_Info
	{
	private:
		User_Info(User_Info const&);
		User_Info& operator=(User_Info const&);
	public:
		connection_ptr connection;
		std::string user_name, room_name;
		// std::shared_ptr<std::thread> thread;
		User_Info() {}
		User_Info(connection_ptr set_connection, const std::string & set_name, const std::string & set_room)
			: connection(set_connection), user_name(set_name), room_name(set_room) {}
		~User_Info()
		{
			// thread.reset();
		}
	};

	// map a username to a Connection and a room name
	std::map<pipedat::ConnectionID, std::unique_ptr<User_Info>> users;
	std::mutex users_mutex;

	// map a room to its users
	std::map<std::string, std::set<pipedat::ConnectionID>> rooms;
	std::mutex room_mutex;

	// a queue for inbound and outbound messages
	threadsafe::queue<Message> input_queue, output_queue;

public:
	Server(const unsigned &height, const unsigned &width);
	~Server() { Console_Framework::restore_console(); }

	void run();

private:
	void listen_for_new_users();

	void receive(const connection_ptr connection);

	void send();

	void handle_events();

	void handle_commands(const connection_ptr connection, const std::vector<std::string> & commands);

	void send_to_room(const std::string & room_name, const std::string & data, const pipedat::ConnectionID & exclude);

	void send_to_user(const pipedat::ConnectionID &user, const std::string &data);

	void remove_user(const connection_ptr connection);
};
