#pragma once

#include <socket-lib.hpp>

#include <map>
#include <set>
#include <string>
#include <mutex>

class Server
{
private:
	// map a username to a Connection
	std::map<std::string, pipedat::Connection> users;
	std::mutex users_mutex;
	
	// map a room to its users
	std::map<std::string, std::set<std::string>> rooms;
	std::mutex room_mutex;

public:
	Server();

	void run();

private:
	void listen_for_new_users();

	void read_from_connection(pipedat::Connection connection);
};