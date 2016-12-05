#pragma once

#include <string>

namespace pipedat
{
	class socket_exception : public std::exception
	{
	public:
		socket_exception(std::string arg) : exception(arg.c_str()) { }
	};

	class connection_exception : public socket_exception
	{
	public:
		connection_exception(std::string arg) : socket_exception(arg) { }
	};
}