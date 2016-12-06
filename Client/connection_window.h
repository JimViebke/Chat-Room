#pragma once
#include "console_framework.h"
#include "constants.h"
#include "tui_element\tui_element.h"

class ConnectionWindow
{
private:
	std::unique_ptr<Text_Box> text_box;
	unsigned height, width;

public:
	ConnectionWindow(const unsigned &height, const unsigned &width);
	~ConnectionWindow() { Console_Framework::restore_console(); }

	std::string run();
};
