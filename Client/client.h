#pragma once

#include "console_framework.h"

class Client
{
private:
	const unsigned HEIGHT = 50, WIDTH = 100;
	const Console_Framework::color_type TEXT_COLOR = Console_Framework::Color::foreground_white | Console_Framework::Color::background_blue;

	const Console_Framework::color_type UI_BACKGROUND = Console_Framework::Color::background_blue;
	const Console_Framework::color_type UI_TEXT_COLOR = Console_Framework::Color::black | Console_Framework::Color::background_white;

public:
	Client();
	
};
