
/* Jim Viebke
Apr 9 2016

The thick wrapper */

#ifndef CONSOLE_FRAMEWORK_H
#define CONSOLE_FRAMEWORK_H

#include <string>
#include <vector>
#include <memory>

#include "console_api.h"

class Console_Framework // the thick wrapper
{
private:

	// store a DWORD representing the original console settings
	static DWORD original_console_mode;

	static unsigned width, height;

	// store HANDLEs to the input and output
	static HANDLE input_handle, output_handle;

	static CONSOLE_SCREEN_BUFFER_INFO originalCSBI;
	static std::vector<CHAR_INFO> originalBuffer;
	static CONSOLE_CURSOR_INFO originalCCI;
	static COORD originalBufferCoord;

	Console_Framework() = delete;

public:

	using color_type = WORD;

	// expose an enum instead of C macros
	enum Color : color_type
	{
		black = 0,

		background_red = BACKGROUND_RED | BACKGROUND_INTENSITY,
		background_green = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		background_blue = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		background_yellow = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		background_white = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,

		foreground_red = FOREGROUND_RED | FOREGROUND_INTENSITY,
		foreground_green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		foreground_blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		foreground_yellow = FOREGROUND_RED | FOREGROUND_GREEN,
		foreground_white = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		foreground_grey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	};

	// publicly define event types
	class Event
	{
	protected:
		Event();
	public:
		virtual ~Event();
	};

	class Done_Event : public Event {};

	class Mouse_Event : public Event
	{
	private:
		bool _left_button_pressed, _right_button_pressed;
		unsigned _buffer_x, _buffer_y;
	public:
		Mouse_Event(const bool & set_left_pressed, const bool & set_right_pressed, const unsigned & set_buffer_x, const unsigned & set_buffer_y);
		bool left_button_pressed() const;
		bool right_button_pressed() const;
		unsigned buffer_x() const; // returns reversed coordinates
		unsigned buffer_y() const;
	};

	class Key_Event : public Event
	{
	private:
		char ascii_char;
	public:
		Key_Event(const char & set_char);
		char get_char() const;
		bool enter_pressed() const;
	};

	class Scroll_Event : public Event
	{
	public:
		enum class Direction { up, down };
		Scroll_Event(const Direction & set_direction);
		Direction get_direction() const;
	private:
		Direction direction;
	};

	// event typedefs
	typedef std::shared_ptr<Event> event_ptr;
	typedef std::shared_ptr<Done_Event> done_event_ptr;
	typedef std::shared_ptr<Mouse_Event> mouse_event_ptr;
	typedef std::shared_ptr<Key_Event> key_event_ptr;
	typedef std::shared_ptr<Scroll_Event> scroll_event_ptr;

	typedef std::vector<event_ptr> event_list;



	static void setup(const unsigned & set_height, const unsigned & set_width, const std::string & title);

	static Console_Framework::event_list get_events();

	static void size_console();

	static void clear_screen();

	static void draw_color(const unsigned & x, const unsigned & y, const Console_Framework::color_type & color);
	static void draw_char(const unsigned & x, const unsigned & y, const char & character, const Console_Framework::color_type & color);
	static void draw_string(const unsigned & x, const unsigned & y, const std::string & data, const color_type & color);
	static void draw_box(const unsigned & x, const unsigned & y, const unsigned & height, const unsigned & width, const color_type & color);

	static void set_cursor_position(const unsigned & x, const unsigned & y);

	static void restore_console();

	static void set_cursor_visibility(const bool & cursor_visible);

	static unsigned get_height();
	static unsigned get_width();

	template <typename Child, typename Parent> static inline std::shared_ptr<Child> convert_to(std::shared_ptr<Parent> const & type)
	{
		// wrap a call to std::dynamic_pointer_cast for convenience
		return std::dynamic_pointer_cast<Child>(type);
	}

	template <typename Child, typename Parent> static inline std::shared_ptr<Child> convert_to(std::shared_ptr<Parent> & type)
	{
		// wrap a call to std::dynamic_pointer_cast for convenience
		return std::dynamic_pointer_cast<Child>(type);
	}
};

#endif
