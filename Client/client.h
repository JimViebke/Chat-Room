#pragma once

#include "../applib-dist/include/console_framework.h"

class Client
{
private:
	const unsigned HEIGHT = 50, WIDTH = 100;
	const Console_Framework::color_type TEXT_COLOR = Console_Framework::Color::foreground_white | Console_Framework::Color::background_blue;
	const Console_Framework::color_type UI_BACKGROUND = Console_Framework::Color::background_blue;

public:
	Client();


};