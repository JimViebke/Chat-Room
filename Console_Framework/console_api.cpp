
/* Jim Viebke
Feb 5 2016 */

#include <console_api.h>

void Console_API::set_console_ctrl_handler(PHANDLER_ROUTINE control_handler)
{
	SetConsoleCtrlHandler(control_handler, TRUE);
}

std::vector<INPUT_RECORD> Console_API::read_console_input(const HANDLE & input_handle)
{
	// create a buffer to hold console input records
	std::vector<INPUT_RECORD> record_buffer(128);
	// create a DWORD to count the number of input records place in the vector
	DWORD event_count;

	// pass required arguments to ReadConsoleInput
	ReadConsoleInput(input_handle, record_buffer.data(), (DWORD)record_buffer.size(), &event_count);

	return record_buffer;
}

void Console_API::get_console_mode(HANDLE & handle, DWORD & mode)
{
	GetConsoleMode(handle, &mode);
}

void Console_API::set_console_mode(HANDLE & handle, DWORD & mode)
{
	SetConsoleMode(handle, mode);
}

void Console_API::write_console_output_attribute(const HANDLE & output_handle, const unsigned & x, const unsigned & y, const WORD & background_color)
{
	// unused but required
	DWORD nCharsWritten;
	// used and required
	WORD color = (WORD)background_color;

	// pass the required arguments to WriteConsoleOutputAttribute
	WriteConsoleOutputAttribute(output_handle, &color, 1, COORD{ (short)x, (short)y }, &nCharsWritten);
}
void Console_API::write_console_output_attribute(const HANDLE & output_handle, const unsigned & x, const unsigned & y, const std::vector<WORD> & attributes)
{
	// unused but required
	DWORD nCharsWritten;
	WriteConsoleOutputAttribute(output_handle, attributes.data(), (DWORD)attributes.size(), COORD{ (short)x, (short)y }, &nCharsWritten);
}

void Console_API::write_console_output_a(HANDLE & output_handle, CHAR_INFO & char_info, COORD & buffer_size, COORD & buffer_coord, SMALL_RECT & write_region)
{
	WriteConsoleOutputA(output_handle, &char_info, buffer_size, buffer_coord, &write_region);
}

void Console_API::write_character_a(HANDLE & output_handle, const unsigned & x, const unsigned & y, const char & character)
{
	DWORD chars;
	WriteConsoleOutputCharacterA(output_handle, &character, 1, COORD{ (short)x, (short)y }, &chars);
}

void Console_API::set_title(const std::string & set_title)
{
	// set the title of the console
	SetConsoleTitleA(set_title.c_str());
}

HANDLE Console_API::get_handle(DWORD handle_type)
{
	return GetStdHandle(handle_type);
}

bool Console_API::is_graph(const char c)
{
	return (isgraph(c)) ? true : false;
}

void Console_API::get_console_screen_buffer_info(HANDLE & output_handle, CONSOLE_SCREEN_BUFFER_INFO & console_buffer)
{
	GetConsoleScreenBufferInfo(output_handle, &console_buffer);
}

void Console_API::read_console_output(HANDLE & output, CHAR_INFO & buffer, COORD & buffer_size, COORD & buffer_coord, SMALL_RECT & read_region)
{
	ReadConsoleOutputA(output, &buffer, buffer_size, buffer_coord, &read_region);
}

void Console_API::get_console_cursor_info(HANDLE & console_output_handle, _CONSOLE_CURSOR_INFO & cursor_info)
{
	GetConsoleCursorInfo(&console_output_handle, &cursor_info);
}

void Console_API::set_console_window_info(HANDLE & output_handle, SMALL_RECT & rectangle)
{
	SetConsoleWindowInfo(output_handle, TRUE, &rectangle);
}

void Console_API::set_console_screen_buffer_size(HANDLE & output_handle, COORD & coord)
{
	SetConsoleScreenBufferSize(output_handle, coord);
}

void Console_API::fill_console_output_attribute(HANDLE & output_handle, const short & color, const unsigned & width, const unsigned & height, COORD & coord)
{
	DWORD characters_written;
	FillConsoleOutputAttribute(output_handle, color, width*height, coord, &characters_written);
}
void Console_API::fill_console_output_character_a(HANDLE & output_handle, const char & c, const unsigned & width, const unsigned & height, COORD & coord)
{
	DWORD characters_written;
	FillConsoleOutputCharacterA(output_handle, c, width*height, coord, &characters_written);
}
void Console_API::write_output_character_a(HANDLE & output_handle, const std::string & data, const unsigned & x, const unsigned & y)
{
	DWORD nCharsWritten;
	WriteConsoleOutputCharacterA(output_handle, data.c_str(), (DWORD)data.size(), COORD{ (short)x, (short)y }, &nCharsWritten);
}

void Console_API::set_console_cursor_position(HANDLE & output_handle, COORD & coord)
{
	SetConsoleCursorPosition(output_handle, coord);
}

void Console_API::show_cursor(HANDLE & output_handle, const bool & show_cursor)
{
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(output_handle, &cursorInfo);
	cursorInfo.bVisible = show_cursor;
	SetConsoleCursorInfo(output_handle, &cursorInfo);
}
