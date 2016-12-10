#pragma once

// standard libraries
#include <mutex>
#include <memory>

// custom libraries
#include <socket-lib.hpp>
#include "console_framework.h"

// local headers
#include "tui_element\tui_element.h"

class Client
{
private:
	std::string user_name = "You";

	std::unique_ptr<Text_Box> text_box;
	std::unique_ptr<Scrollable_Text_Display> display;
	std::mutex display_mutex;

	std::unique_ptr<pipedat::Connection> connection;

public:
	Client(const unsigned &height, const unsigned &width, std::unique_ptr<pipedat::Connection> &connection);
	~Client() { Console_Framework::restore_console(); }

	void run();
	void receive();
};
