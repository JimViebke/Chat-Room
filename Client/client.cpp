#include "client.h"
#include "constants.h"

#include <thread>
#include <iostream>
#include <sstream>
#include <iterator>

Client::Client(const unsigned &height, const unsigned &width, const std::string & ip, const unsigned & port)
{
	// configure the console
	Console_Framework::setup(height, width, "Chat Room");

	// draw the blue box
	Console_Framework::draw_box(height - 3, 0, 3, width, Constants::UI_BACKGROUND);

	// draw the prompt
	Console_Framework::draw_char(height - 2, 1, '>', Constants::TEXT_COLOR);

	// create our TUI elements
	text_box = std::make_unique<Text_Box>(height - 2, 3, width - 4, Constants::UI_TEXT_COLOR);
	display = std::make_unique<Scrollable_Text_Display>(1, 1, height - 4, width - 2);

	// start the cursor in the textbox
	Console_Framework::set_cursor_visibility(true);
	Console_Framework::set_cursor_position(height - 2, 3);

	try
	{
		connection = std::make_unique<pipedat::Connection>(ip, port);
	}
	catch (std::exception & ex)
	{
		this->display->add(ex.what());
		std::cin.ignore();
	}
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

					std::stringstream ss(message);
					const std::istream_iterator<std::string> begin(ss);
					std::vector<std::string> strings(begin, std::istream_iterator<std::string>());

					if (strings.size() > 1 && strings[0] == "/name")
						user_name = message.substr(6, message.size());

					display->add(user_name + ": " + message);

					if (message == "/exit")
					{
						connection->send("");
						return;
					}
					else if (message == "/help")
					{
						display->add("Use one of the following commands:");
						display->add("    up arrow key - Used to scroll up through previous messages in the chat room.");
						display->add("    down arrow key - Used to scroll down through messages in the chat room.");
						display->add("    /name - Used in conjunction with a new name to change your name.");
						display->add("    /join - Used in conjunction with a room name to join a new room.");
						display->add("    /exit - Used to exit the application.");
					}

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
	try
	{
		for (;;)
		{
			const std::string message = connection->receive();
			std::lock_guard<std::mutex> lock(display_mutex);
			display->add(message);
		}
	}
	catch (pipedat::disgraceful_disconnect_exception)
	{
		display->add("The server has been shutdown by the host.");
	}
	catch (std::exception)
	{
		display->add("The server has unexpectedly shutdown.");
	}
}
