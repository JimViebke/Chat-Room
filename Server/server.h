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
	using user_data = std::pair<connection_ptr, std::string>;

	// map a username to a Connection and a room name
	std::map<pipedat::ConnectionID, user_data> users;
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
};
