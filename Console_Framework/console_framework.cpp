
/* Jim Viebke
Feb 5 2016 */

#include <console_framework.h>

#include <iostream>

namespace // anonymous namespace
{
	bool done = false;

	int control_handler(DWORD ctrlType); // forward-declaring
}

// console framework data

DWORD Console_Framework::original_console_mode;

unsigned Console_Framework::width, Console_Framework::height;

CONSOLE_SCREEN_BUFFER_INFO Console_Framework::originalCSBI;
std::vector<CHAR_INFO> Console_Framework::originalBuffer;
CONSOLE_CURSOR_INFO Console_Framework::originalCCI;
COORD Console_Framework::originalBufferCoord;

HANDLE Console_Framework::input_handle, Console_Framework::output_handle;

// event types

Console_Framework::Event::Event() {}
Console_Framework::Event::~Event() {}

Console_Framework::Mouse_Event::Mouse_Event(const bool & set_left_pressed, const bool & set_right_pressed, const unsigned & set_buffer_x, const unsigned & set_buffer_y) :
	_left_button_pressed(set_left_pressed), _right_button_pressed(set_right_pressed), _buffer_x(set_buffer_x), _buffer_y(set_buffer_y) {}

bool Console_Framework::Mouse_Event::left_button_pressed() const { return _left_button_pressed; }
bool Console_Framework::Mouse_Event::right_button_pressed() const { return _right_button_pressed; }
unsigned Console_Framework::Mouse_Event::buffer_x() const { return _buffer_y; } // reverse coordinates
unsigned Console_Framework::Mouse_Event::buffer_y() const { return _buffer_x; }

Console_Framework::Key_Event::Key_Event(const char & set_char) : ascii_char(set_char) {}
char Console_Framework::Key_Event::get_char() const { return ascii_char; }
bool Console_Framework::Key_Event::enter_pressed() const { return ascii_char == '\n' || ascii_char == '\r'; }

Console_Framework::Scroll_Event::Scroll_Event(const Direction & set_direction) : direction(set_direction) {}
Console_Framework::Scroll_Event::Direction Console_Framework::Scroll_Event::get_direction() const { return direction; }

// console framework API implementations

void Console_Framework::setup(const unsigned & set_height, const unsigned & set_width, const std::string & title)
{
	height = set_height;
	width = set_width;

	// save the input and output handles
	input_handle = Console_API::get_handle(STD_INPUT_HANDLE);
	output_handle = Console_API::get_handle(STD_OUTPUT_HANDLE);

	// save the original console mode
	Console_API::get_console_mode(input_handle, Console_Framework::original_console_mode);

	// create console in our own image
	{ // temporary scope to destroy mode
		DWORD mode = ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS; // fixed for Win10
		Console_API::set_console_mode(input_handle, mode);
	}

	Console_Framework::size_console();

	// set a control handler
	Console_API::set_console_ctrl_handler((PHANDLER_ROUTINE)control_handler);

	// set the title of the console
	Console_API::set_title(title);

	// clear the screen
	Console_Framework::clear_screen();
}

Console_Framework::event_list Console_Framework::get_events()
{
	std::vector<INPUT_RECORD> record_buffer = Console_API::read_console_input(input_handle);

	// createa vector of shared_ptr's to event types
	event_list console_events;

	// for each event that occured
	for (unsigned i = 0; i < record_buffer.size(); ++i)
	{
		switch (record_buffer[i].EventType)
		{
		case MOUSE_EVENT:
		{
			// create boolean values to indicate if the left or right mouse buttons are down
			bool left_button_pressed = false, right_button_pressed = false;

			// extract the the mouse event and event flags
			const MOUSE_EVENT_RECORD mouse_event = record_buffer[i].Event.MouseEvent;

			if (mouse_event.dwEventFlags == 0) // detect a click event
			{
				// test if the left mouse button is pressed
				left_button_pressed = (mouse_event.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) ? true : false;
				// test if the right mouse button is pressed
				right_button_pressed = (mouse_event.dwButtonState & RIGHTMOST_BUTTON_PRESSED) ? true : false;
			}

			// create a new mouse event using the two booleans and the x,y position, and add it to the event vector
			console_events.push_back(std::make_shared<Mouse_Event>(
				left_button_pressed, right_button_pressed,
				mouse_event.dwMousePosition.X,
				mouse_event.dwMousePosition.Y));
			break;
		}
		case KEY_EVENT:
		{
			// extract the key event record
			const KEY_EVENT_RECORD key_event_record = record_buffer[i].Event.KeyEvent;

			// if a key has been pressed
			if (key_event_record.bKeyDown)
			{
				if (key_event_record.wVirtualKeyCode == VK_UP)
					console_events.push_back(std::make_shared<Scroll_Event>(Scroll_Event::Direction::up));
				else if (key_event_record.wVirtualKeyCode == VK_DOWN)
					console_events.push_back(std::make_shared<Scroll_Event>(Scroll_Event::Direction::down));
				else // create a new key event using the ascii character and the control key state
					console_events.push_back(std::make_shared<Key_Event>(key_event_record.uChar.AsciiChar));
			}

			break;
		}
		}
	}

	// if the done flag has been set to true by the control handler
	if (done)
	{
		// pass a Done event to the caller
		console_events.push_back(std::make_shared<Done_Event>());
	}

	// return the vector of console events
	return console_events;
}

void Console_Framework::size_console()
{
	WORD currentConsoleWidth;

	Console_API::get_console_screen_buffer_info(output_handle, originalCSBI);

	// save the desktop
	originalBuffer.resize(originalCSBI.dwSize.X*originalCSBI.dwSize.Y);
	originalBufferCoord = COORD{ 0 };
	SMALL_RECT bufferRect{ 0 };
	bufferRect.Bottom = originalCSBI.dwSize.Y - 1;
	bufferRect.Right = originalCSBI.dwSize.X - 1;
	Console_API::read_console_output(output_handle, *originalBuffer.data(), originalCSBI.dwSize, originalBufferCoord, bufferRect);

	// save the cursor
	Console_API::get_console_cursor_info(output_handle, originalCCI);

	SMALL_RECT sr{ 0 };
	Console_API::set_console_window_info(output_handle, sr);

	Console_API::set_console_screen_buffer_size(output_handle, COORD{ (short)width, (short)height });

	CONSOLE_SCREEN_BUFFER_INFO sbi;
	Console_API::get_console_screen_buffer_info(output_handle, sbi);
	sr.Top = sr.Left = 0;
	width = std::min((SHORT)width, sbi.dwMaximumWindowSize.X);
	height = std::min((SHORT)height, sbi.dwMaximumWindowSize.Y);
	sr.Right = width - 1;
	sr.Bottom = height - 1;

	Console_API::set_console_window_info(output_handle, sr);
	currentConsoleWidth = sr.Right - sr.Left + 1;

	auto newCCI = originalCCI;
	newCCI.bVisible = FALSE;
	Console_API::get_console_cursor_info(output_handle, newCCI);
}

void Console_Framework::clear_screen()
{
	// overwrite the console
	Console_API::fill_console_output_attribute(output_handle, (char)Color::black, width, height, COORD{ 0, 0 });
	Console_API::fill_console_output_character_a(output_handle, (char)Color::black, width, height, COORD{ 0, 0 });
}

void Console_Framework::draw_color(const unsigned & x, const unsigned & y, const Console_Framework::color_type & color)
{
	// forward calls to write_console_output_attribute in the thin wrapper
	// we're also changing the x,y format here to the API's type
	Console_API::write_console_output_attribute(output_handle, y, x, (WORD)color);
}
void Console_Framework::draw_char(const unsigned & x, const unsigned & y, const char & character, const Console_Framework::color_type & color)
{
	// forward calls to write_console_output_a in the thin wrapper

	// draw foreground color
	draw_color(x, y, color);

	// we're also changing the x,y format here to the API's type
	Console_API::write_character_a(output_handle, y, x, character);
}
void Console_Framework::draw_string(const unsigned & x, const unsigned & y, const std::string & data, const color_type & color)
{
	std::vector<WORD> attributes(data.size(), color);
	Console_API::write_output_character_a(output_handle, data, y, x); // invert x and y
	Console_API::write_console_output_attribute(output_handle, y, x, attributes); // invert x and y
}
void Console_Framework::draw_box(const unsigned & x, const unsigned & y, const unsigned & height, const unsigned & width, const color_type & background_color)
{
	for (unsigned i = 0; i < height; ++i)
		draw_string(x + i, y, std::string(width, ' '), background_color);
}

void Console_Framework::set_cursor_position(const unsigned & x, const unsigned & y)
{
	// again, reversing coordinates for the API
	Console_API::set_console_cursor_position(output_handle, COORD{ (short)y, (short)x });
}

void Console_Framework::restore_console()
{
	// clear the buffer
	Console_Framework::clear_screen();

	// restore the previous state of the console
	SMALL_RECT sr{ 0 };
	Console_API::set_console_window_info(output_handle, sr);
	Console_API::set_console_screen_buffer_size(output_handle, originalCSBI.dwSize);
	Console_API::set_console_window_info(output_handle, originalCSBI.srWindow);

	// restore the desktop contents
	Console_API::write_console_output_a(output_handle, *originalBuffer.data(), originalCSBI.dwSize, originalBufferCoord, originalCSBI.srWindow);

	// restore the cursor
	Console_API::get_console_cursor_info(output_handle, originalCCI);
	Console_API::set_console_cursor_position(output_handle, originalCSBI.dwCursorPosition);

	// clean up when the console falls out of scope
	Console_API::set_console_mode(input_handle, original_console_mode);
}

void Console_Framework::set_cursor_visibility(const bool & cursor_visible)
{
	Console_API::show_cursor(output_handle, cursor_visible);
}

unsigned Console_Framework::get_height()
{
	return height;
}
unsigned Console_Framework::get_width()
{
	return width;
}

namespace // anonymous namespace
{
	int control_handler(DWORD ctrlType)
	{
		switch (ctrlType) {
			case CTRL_C_EVENT:
			{
				// set the done flag to true
				done = true;

				// restore the console's previous settings
				Console_Framework::restore_console();

				return 1;
			}
		}
		return 0;
	}
}
