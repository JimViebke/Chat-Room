
#include "tui_element.h"


/* Jim Viebke
Apr 9 2016 */

#include "tui_element.h"

TUI_Element::TUI_Element(const unsigned & set_x, const unsigned & set_y) : _x(set_x), _y(set_y) {}



Text_Box::Text_Box(const unsigned & set_x, const unsigned & set_y, const unsigned & set_width, const Console_Framework::color_type set_text_color)
	: TUI_Element(set_x, set_y), _width(set_width), text_color(set_text_color)
{
	Console_Framework::draw_box(_x, _y, 1, _width, Console_Framework::Color::background_white);
}

void Text_Box::add_char(const char & character)
{
	data += character;
	render();
}
void Text_Box::backspace()
{
	if (data.size() > 0) data.erase(--data.cend());
	render();
}
std::string Text_Box::get_contents() const
{
	return data;
}
void Text_Box::clear()
{
	data.clear();
	render();
}
void Text_Box::render() const
{
	// if the text is too long to fit in the label
	if (data.size() > _width - 1)
		for (unsigned i = 0; i < _width - 1; ++i) // render last _width characters
			Console_Framework::draw_char(_x, _y + i, data[data.size() - _width + 1 + i], text_color);
	else // the text fits within the label
		for (unsigned i = 0; i < std::min((size_t)_width - 1, data.size()); ++i) // render all characters
			Console_Framework::draw_char(_x, _y + i, data[i], text_color);

	// set the cursor to visible
	Console_Framework::set_cursor_visibility(true);

	// set the cursor to the end of the text
	Console_Framework::set_cursor_position(_x, _y + std::min((unsigned)data.size(), _width - 1));

	// clear the next character
	if (data.size() < _width)
		Console_Framework::draw_string(_x, _y + (unsigned)data.size(), " ", text_color);
}



void Scrollable_Text_Display::render()
{
	// clear the viewer
	Console_Framework::draw_box(_x, _y, _height, _width, Console_Framework::black);

	if (data.size() == 0) return;

	// render the text
	for (unsigned i = 0; i < std::min(_height, (unsigned)data.size() - _scroll_height); ++i)
		Console_Framework::draw_string(_x + i, _y, data[i + _scroll_height].substr(0, std::min(_width, (unsigned)data[i + _scroll_height].size())), Console_Framework::foreground_white);
}

Scrollable_Text_Display::Scrollable_Text_Display(const unsigned & set_x, const unsigned & set_y, const unsigned & set_height, const unsigned & set_width)
	: TUI_Element(set_x, set_y), _height(set_height), _width(set_width) {}

void Scrollable_Text_Display::add(const std::string & add)
{
	data.push_back(add);
}
void Scrollable_Text_Display::scroll(const Console_Framework::Scroll_Event::Direction & direction)
{
	// The following logic allows for scrolling three lines at a time, with the bottom
	// line being allowed to scroll up to the first row in the container.

	if (direction == Console_Framework::Scroll_Event::Direction::up)
	{
		if (_scroll_height <= 3) _scroll_height = 0;
		else _scroll_height -= 3;
	}
	else if (direction == Console_Framework::Scroll_Event::Direction::down)
	{
		_scroll_height += 3;

		if (data.size() >= 1 && _scroll_height >= data.size())
		{
			_scroll_height = std::min(_scroll_height, (unsigned)data.size());
			if (_scroll_height > 1)
				--_scroll_height;
		}
	}

	render();
}
void Scrollable_Text_Display::clear()
{
	data.clear();
	render();
}