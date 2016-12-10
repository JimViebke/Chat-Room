#pragma once
#include <socket-lib.hpp>
#include "console_framework.h"
#include "constants.h"
#include "tui_element\tui_element.h"

#include <memory>
#include <string>

class ConnectionWindow
{
private:
	std::unique_ptr<Text_Box> text_box;
	unsigned height, width;

	std::string is_valid_input(const std::string &ip_and_port);

public:
	ConnectionWindow(const unsigned &height, const unsigned &width);
	~ConnectionWindow() { Console_Framework::restore_console(); }

	std::unique_ptr<pipedat::Connection> run();
};
