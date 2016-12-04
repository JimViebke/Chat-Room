
#include "client.h"

Client::Client()
{
	// configure the console
	Console_Framework::setup(HEIGHT, WIDTH, "Chat Room");

	// create the blue boxes
	Console_Framework::draw_box(0, 0, 3, WIDTH, UI_BACKGROUND);
	Console_Framework::draw_box(HEIGHT - 1, 0, 1, WIDTH, UI_BACKGROUND);

	// draw the labels
	Console_Framework::draw_string(1, 1, "Path:", TEXT_COLOR);
	Console_Framework::draw_string(1, 63, "Regex:", TEXT_COLOR);
	Console_Framework::draw_string(1, 87, "Recursive?", TEXT_COLOR);
	Console_Framework::draw_string(HEIGHT - 1, 1, "File/folder count:", TEXT_COLOR);
	Console_Framework::draw_string(HEIGHT - 1, 33, "Selected count:", TEXT_COLOR);
	Console_Framework::draw_string(HEIGHT - 1, 66, "Selected size:", TEXT_COLOR);

	// hide the cursor until the user selects a textbox
	Console_Framework::set_cursor_visibility(false);
}
