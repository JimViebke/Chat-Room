#pragma once

#include "console_framework.h"

class Constants
{
public:
	// Console text color
	const static Console_Framework::color_type TEXT_COLOR = Console_Framework::Color::foreground_white | Console_Framework::Color::background_blue;

	// Console background colors
	const static Console_Framework::color_type UI_BACKGROUND = Console_Framework::Color::background_blue;
	const static Console_Framework::color_type UI_TEXT_COLOR = Console_Framework::Color::black | Console_Framework::Color::background_white;
};