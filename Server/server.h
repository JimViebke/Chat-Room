#pragma once

#include <socket-lib.hpp>

#include <map>
#include <set>
#include <string>
#include <mutex>

#include "threadsafe_queue.h"

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

	using connection_ptr = std::shared_ptr<pipedat::Connection>;

	class User_Info
	{
	public:
		connection_ptr connection;
		std::string user_name, room_name;
		User_Info() {}
		User_Info(connection_ptr set_connection, const std::string & set_name, const std::string & set_room)
			: connection(set_connection), user_name(set_name), room_name(set_room) {}
	};

	// map a username to a Connection and a room name
	std::map<pipedat::ConnectionID, User_Info> users;
	std::mutex users_mutex;

	// map a room to its users
	std::map<std::string, std::set<pipedat::ConnectionID>> rooms;
	std::mutex room_mutex;

	threadsafe::queue<Message> input_queue, output_queue;

public:
	Server();

	void run();

private:
	void listen_for_new_users();

	void receive(connection_ptr connection);

	void send();

	void handle_commands(connection_ptr connection, const std::vector<std::string> & commands);

	void send_to_room(const std::string & room_name, const std::string & data, const pipedat::ConnectionID & exclude);
};
