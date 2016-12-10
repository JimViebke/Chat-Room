#pragma once

#pragma once

/* Jim Viebke
Apr 9 2016 */

#include <string>

#include "console_framework.h"

class TUI_Element
{
protected:
	unsigned _x, _y;

	TUI_Element(const unsigned & set_x, const unsigned & set_y);
};

class Text_Box : public TUI_Element
{
private:
	unsigned _width; // the width of the text box
	std::string data;
	Console_Framework::color_type text_color;

public:
	Text_Box(const unsigned & set_x, const unsigned & set_y, const unsigned & set_width, const Console_Framework::color_type & set_text_color);

	void add_char(const char & character);
	void backspace();
	std::string take_contents();
	void clear();

private:
	void render() const;
};

class Scrollable_Text_Display : public TUI_Element
{
private:
	class Line
	{
	public:
		std::string data;
		Console_Framework::color_type color;
		Line(const std::string & set_data, const Console_Framework::color_type & set_color);
	};

	unsigned _height, _width, _scroll_height = 0;
	std::vector<Line> data;

	void render();

public:
	Scrollable_Text_Display(const unsigned & set_x, const unsigned & set_y, const unsigned & set_height, const unsigned & set_width);

	void add(const std::string & add, const Console_Framework::color_type & color);
	void scroll(const Console_Framework::Scroll_Event::Direction & direction);
	void clear();
};