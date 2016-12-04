
/* Jim Viebke
Apr 9 2016

The thin wrapper */

#ifndef CONSOLE_API_H
#define CONSOLE_API_H

// C includes
#include <Windows.h>
#undef min

// C++ includes
#include <algorithm>
#include <memory>
#include <vector>

class Console_API // the thin wrapper
{
public:
	static std::vector<INPUT_RECORD> read_console_input(const HANDLE & input_handle);

	static void get_console_mode(HANDLE & handle, DWORD & mode);

	static void set_console_mode(HANDLE & handle, DWORD & mode);

	static void write_console_output_attribute(const HANDLE & output_handle, const unsigned & x, const unsigned & y, const WORD & background_color);
	static void write_console_output_attribute(const HANDLE & output_handle, const unsigned & x, const unsigned & y, const std::vector<WORD> & attributes);

	static void write_console_output_a(HANDLE & output_handle, CHAR_INFO & char_info, COORD & buffer_size, COORD & buffer_coord, SMALL_RECT & write_region);

	static void write_character_a(HANDLE & output_handle, const unsigned & x, const unsigned & y, const char & character);

	static void set_title(const std::string & set_title);

	static void set_console_ctrl_handler(PHANDLER_ROUTINE control_handler);

	static HANDLE get_handle(DWORD handle_type);

	static bool is_graph(const char c);

	static void get_console_screen_buffer_info(HANDLE & output_handle, CONSOLE_SCREEN_BUFFER_INFO & console_buffer);

	static void read_console_output(HANDLE & output, CHAR_INFO & buffer, COORD & buffer_size, COORD & buffer_coord, SMALL_RECT & read_region);

	static void get_console_cursor_info(HANDLE & console_output_handle, _CONSOLE_CURSOR_INFO & cursor_info);

	static void set_console_window_info(HANDLE & output_handle, SMALL_RECT & rectangle);

	static void set_console_screen_buffer_size(HANDLE & output_handle, COORD & coord);

	static void fill_console_output_attribute(HANDLE & output_handle, const short & color, const unsigned & width, const unsigned & height, COORD & coord);
	static void fill_console_output_character_a(HANDLE & output_handle, const char & c, const unsigned & width, const unsigned & height, COORD & coord);
	static void write_output_character_a(HANDLE & output_handle, const std::string & data, const unsigned & x, const unsigned & y);

	static void set_console_cursor_position(HANDLE & output_handle, COORD & coord);
	
	static void show_cursor(HANDLE & output_handle, const bool & show_cursor);
};

#endif
