#pragma once

// standard libraries
#include <mutex>

// custom libraries
#include <socket-lib.hpp>
#include "console_framework.h"

// local headers
#include "tui_element\tui_element.h"

class Client
{
private:
	const unsigned HEIGHT = 50, WIDTH = 100;
	const Console_Framework::color_type TEXT_COLOR = Console_Framework::Color::foreground_white | Console_Framework::Color::background_blue;

	const Console_Framework::color_type UI_BACKGROUND = Console_Framework::Color::background_blue;
	const Console_Framework::color_type UI_TEXT_COLOR = Console_Framework::Color::black | Console_Framework::Color::background_white;

	std::string user_name = "User";

	std::unique_ptr<Text_Box> text_box;
	std::unique_ptr<Scrollable_Text_Display> display;
	std::mutex display_mutex;

	std::unique_ptr<pipedat::Connection> connection;

public:
	Client(const std::string & ip, const unsigned & port);

	void run();
	void receive();
};
