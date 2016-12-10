#include "connection_window.h"

#include <string>
#include <sstream>
#include <vector>

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

ConnectionWindow::ConnectionWindow(const unsigned &height, const unsigned &width)
{
	ConnectionWindow::height = height;
	ConnectionWindow::width = width;

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
	text_box = std::make_unique<Text_Box>(unsigned(height - ystart), unsigned(label.size() + padding), unsigned(width - label.size() - padding - 2), Constants::UI_TEXT_COLOR);

	// Start the cursor in the textbox
	Console_Framework::set_cursor_visibility(true);
	Console_Framework::set_cursor_position(unsigned(height - ystart), unsigned(label.size() + padding));
}

std::unique_ptr<pipedat::Connection> ConnectionWindow::run()
{
	for (;;)
	{
		// get the list of events that have occurred
		const Console_Framework::event_list events = Console_Framework::get_events();

		// for each event
		for (const Console_Framework::event_ptr & event : events)
		{
			if (const Console_Framework::done_event_ptr done = Console_Framework::convert_to<Console_Framework::Done_Event>(event))
			{
				return nullptr;
			}
			else if (const Console_Framework::key_event_ptr key_event = Console_Framework::convert_to<Console_Framework::Key_Event>(event))
			{
				// if the enter key was pressed
				if (key_event->enter_pressed())
				{
					// read the message
					std::string error;
					std::string ip_and_port = text_box->take_contents();

					// Check if the entered string is in a valid format
					error = is_valid_input(ip_and_port);

					try
					{
						if (error == "")
						{
							const std::string ip_address = ip_and_port.substr(0, ip_and_port.find("::"));
							const std::string p = ip_and_port.substr(ip_and_port.find("::") + 2, ip_and_port.size());
							const unsigned port = stoi(p);

							std::unique_ptr<pipedat::Connection> connection = std::make_unique<pipedat::Connection>(ip_address, port);

							return std::move(connection);
						}
						else
							throw std::exception();
					}
					catch (std::exception)
					{
						if(error == "")
							error = "error: Could not bind to ipaddress and port.";

						// Clear the previous error if there is one
						for (unsigned i = 0; i < width; ++i)
							Console_Framework::draw_char(height - 1, (i + 1), ' ', Constants::TEXT_COLOR);

						// Write the new error to console
						for (unsigned i = 0; i < error.size(); ++i)
							Console_Framework::draw_char(height - 1, (i + 1), error[i], Constants::TEXT_COLOR);
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

std::string ConnectionWindow::is_valid_input(const std::string &ip_and_port)
{
	// Verify the ip address
	if (ip_and_port.find("::") == std::string::npos)
		return "error: Entered string was not in format ipaddress::port.";
	else
	{
		// Get the ipaddress and port number
		std::string ip_address = ip_and_port.substr(0, ip_and_port.find("::"));
		std::string s_port = ip_and_port.substr(ip_and_port.find("::") + 2, ip_and_port.size());

		try
		{
			// This will thorw an error if the port number is not a number
			unsigned uport = stoi(s_port);

			try
			{
				std::vector<std::string> pieces;
				split(ip_address, '.', pieces);

				if (pieces.size() != 4)
					throw std::exception();

				unsigned piece;
				for (std::string p : pieces)
				{
					// The following will throw an exception to be caught if pieces cannot be put into an integer
					piece = stoi(p);

					// A single piece should not be over 255
					if (piece > 255)
						throw std::exception();
				}

				// There are no issues with the entered string
				return "";
			}
			catch (std::exception ex)
			{
				return "error: Invalid ipaddress.";
			}
		}
		catch (std::exception ex)
		{
			return "error: Invalid port number.";
		}
	}
}