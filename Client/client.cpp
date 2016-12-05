
#include <thread>

#include "client.h"

Client::Client(const std::string ip, const unsigned & port)
{
	// configure the console
	Console_Framework::setup(HEIGHT, WIDTH, "Chat Room");

	// draw the blue box
	Console_Framework::draw_box(HEIGHT - 3, 0, 3, WIDTH, UI_BACKGROUND);

	// draw the prompt
	Console_Framework::draw_char(HEIGHT - 2, 1, '>', TEXT_COLOR);

	// create our TUI elements
	text_box = std::make_unique<Text_Box>(HEIGHT - 2, 3, WIDTH - 4, UI_TEXT_COLOR);
	display = std::make_unique<Scrollable_Text_Display>(1, 1, HEIGHT - 4, WIDTH - 2);

	// start the cursor in the textbox
	Console_Framework::set_cursor_visibility(true);
	Console_Framework::set_cursor_position(HEIGHT - 2, 3);

	connection = std::make_unique<pipedat::Connection>(ip, port);
}

void Client::run()
{
	// start the receiving thread here
	std::thread(&Client::receive, this).detach();

	for (;;)
	{
		// get the list of events that have occurred
		const Console_Framework::event_list events = Console_Framework::get_events();

		// for each event
		for (const Console_Framework::event_ptr & event : events)
		{
			// if the event is a done event type
			if (const Console_Framework::done_event_ptr done_event = Console_Framework::convert_to<Console_Framework::Done_Event>(event))
			{
				return; // somehow kill the other thread (or just let it die)
			}
			else if (const Console_Framework::scroll_event_ptr scroll_event = Console_Framework::convert_to<Console_Framework::Scroll_Event>(event))
			{
				// scroll and re-render the viewer
				std::lock_guard<std::mutex> lock(display_mutex);
				this->display->scroll(scroll_event->get_direction());
			}
			else if (const Console_Framework::key_event_ptr key_event = Console_Framework::convert_to<Console_Framework::Key_Event>(event))
			{
				// if the enter key was pressed
				if (key_event->enter_pressed())
				{
					// read the message
					const std::string message = text_box->take_contents();

					// add the message to the sender's screen (better than having the server send it back)		
					std::lock_guard<std::mutex> lock(display_mutex);
					display->add(user_name + ": " + message);

					// send the message
					connection->send(message);
				}
				else // all other key events
				{
					// append the char to the selected textbox's contents and re-render
					text_box->add_char(key_event->get_char());
				}
			}
		}
	}
}

void Client::receive()
{
	for (;;)
	{
		const std::string message = connection->receive();
		std::lock_guard<std::mutex> lock(display_mutex);
		display->add(message);
	}
}
