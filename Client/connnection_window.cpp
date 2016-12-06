#include "connection_window.h"

ConnectionWindow::ConnectionWindow(const unsigned &height, const unsigned &width)
{
	ConnectionWindow::height = height;
	ConnectionWindow::width = height;

	unsigned ystart = 3;
	unsigned padding = 2;

	// Configure the console
	Console_Framework::setup(height, width, "Connection");

	// Draw the blue box
	Console_Framework::draw_box(height - 4, 0, 4, width, Constants::UI_BACKGROUND);

	// Draw the prompt
	std::string label = "ipaddress::port";
	for (unsigned i = 0; i < label.size(); ++i)
	{
		Console_Framework::draw_char((height - ystart), (i + 1), label[i], Constants::TEXT_COLOR);
	}

	// Create our TUI elements
	text_box = std::make_unique<Text_Box>((height - ystart), (label.size() + padding), (width - label.size() - padding - 2), Constants::UI_TEXT_COLOR);

	// Start the cursor in the textbox
	Console_Framework::set_cursor_visibility(true);
	Console_Framework::set_cursor_position((height - ystart), (label.size() + padding));
}

std::string ConnectionWindow::run()
{
	for (;;)
	{
		// get the list of events that have occurred
		const Console_Framework::event_list events = Console_Framework::get_events();

		// for each event
		for (const Console_Framework::event_ptr & event : events)
		{
			if (const Console_Framework::key_event_ptr key_event = Console_Framework::convert_to<Console_Framework::Key_Event>(event))
			{
				// if the enter key was pressed
				if (key_event->enter_pressed())
				{
					// read the message
					std::string error, ip_address;
					unsigned port;
					std::string ip_and_port = text_box->take_contents();

					if (ip_and_port.find("::") == std::string::npos)
					{
						error = "error: No '::' separator between ipaddress and port.";
						continue;
					}

					// If there was an error, display that error
					if (error.size() != 0)
					{
						for (unsigned i = 0; i < error.size(); ++i)
						{
							Console_Framework::draw_char(height - 1, (i + 1), error[i], Constants::TEXT_COLOR);
						}
					}
					else
					{
						return ip_and_port;
					}
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