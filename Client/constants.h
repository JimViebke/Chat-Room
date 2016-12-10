#pragma once

#include "console_framework.h"

class Constants
{
public:
	// Console text color
	const static Console_Framework::color_type TEXT_COLOR = Console_Framework::Color::foreground_white | Console_Framework::Color::background_blue;

	// Console background colors
	const static Console_Framework::color_type
		UI_BACKGROUND = Console_Framework::Color::background_blue,
		UI_TEXT_COLOR = Console_Framework::Color::black | Console_Framework::Color::background_white,

		TEXT_DEFAULT = Console_Framework::Color::foreground_white | Console_Framework::Color::black,
		TEXT_WHISPER = Console_Framework::Color::foreground_green | Console_Framework::Color::black,
		TEXT_INFO = Console_Framework::Color::foreground_grey | Console_Framework::Color::black,
		TEXT_ERROR = Console_Framework::Color::foreground_red | Console_Framework::Color::black;

	const static std::string
		HELP_FLAG,
		INFO_FLAG,
		WHISPER_FLAG;

};

using C = Constants; // create an abbreviation
