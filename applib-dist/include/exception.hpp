#pragma once

#include <string>

namespace pipedat
{
	// Generic exception for all socket library exceptions to inherit from
	class socket_exception : public std::exception
	{
	public:
		socket_exception(std::string arg) : exception(arg.c_str()) { }
	};

	// WSA startup failures
	class connection_exception : public socket_exception
	{
	public:
		connection_exception(std::string arg) : socket_exception(arg) { }
	};

	// Failure to start listening on port
	class listen_exception : public socket_exception
	{
	public:
		listen_exception(std::string arg) : socket_exception(arg) { }
	};

	// Client disgraceful disconnect exception
	class disgraceful_disconnect_exception : public socket_exception
	{
	public:
		disgraceful_disconnect_exception(std::string arg) : socket_exception(arg) { }
	};
}