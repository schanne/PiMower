#include "UIText.h"

std::map<eUITextFont, UITextFont> UIText::fonts;

UIText::UIText(Rectangle location, std::string text, Theme *theme, eThemeObjState state) :
	UIElement(Point(location.x1, location.y1), false, false, theme)
{
	this->buttonText = text;

	// Location denotes the CENTER of the text
	themeSettings = theme->LoadTheme(eThemeObjType::clickable, state);
	setButtonLines();
	loadFonts();
}

void UIText::setTouchArea()
{
	//UIText is not touchableArea
	touchArea = NULL;
}

void UIText::setButtonLines()
{
	buttonTextLines.clear();
	std::istringstream ss(buttonText);
	std::string s;
	while (getline(ss, s, '\n'))
	{
		buttonTextLines.push_back(s);
	}
}

// Used to figure out how big the text will be, this is also called by
// the constructor to define the actual size of this
Rectangle UIText::GetFontRect(std::string text, eUITextFont font)
{
	return GetFontRect(text, font, 0, 0);
}

Rectangle UIText::GetFontRect(std::string text, eUITextFont font, uint8_t interlineSpacing, uint8_t textSpacing)
{
	loadFonts();
	uint8_t charWidth = fonts[font].width;
	uint8_t charHeight = fonts[font].height;
	std::istringstream ss(text);
	std::string s;
	std::vector<std::string> lines;

	while (getline(ss, s, '\n'))
	{
		lines.push_back(s);
	}

	// # of lines denotes the height
	int height = (charHeight*lines.size()) + ((lines.size() - 1)*interlineSpacing);
	// Set the length to the maximum length line
	uint8_t lineLength = 0;
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		if (lines[i].length() > lineLength)
			lineLength = lines[i].length();
	}
	int width = (charWidth*lineLength) + ((lineLength - 1)*textSpacing);
	return (Rectangle(0, 0, width, height));
}

void UIText::loadFonts()
{
	if (fonts.size() > 0)
	{
		return;
	}
	UITextFont font;
#ifdef useAlef10
	font.width = 9;
	font.height = 14;
	fonts.insert(std::make_pair(eUITextFont::Alef10, font));
#endif

#ifdef useAlef14
	font.width = 13;
	font.height = 19;
	fonts.insert(std::make_pair(eUITextFont::Alef14, font));
#endif

#ifdef useAlef24
	font.width = 22;
	font.height = 32;
	fonts.insert(std::make_pair(eUITextFont::Alef24, font));
#endif

#ifdef useAlef36
	font.width = 35;
	font.height = 43;
	fonts.insert(std::make_pair(eUITextFont::Alef36, font));
#endif

#ifdef useAlefB10
	font.width = 11;
	font.height = 14;
	fonts.insert(std::make_pair(eUITextFont::AlefB10, font));
#endif

#ifdef useAlefB14
	font.width = 13;
	font.height = 19;
	fonts.insert(std::make_pair(eUITextFont::AlefB14, font));
#endif

#ifdef useAlefB24
	font.width = 22;
	font.height = 32;
	fonts.insert(std::make_pair(eUITextFont::AlefB24, font));
#endif

#ifdef useAlefB36
	font.width = 33;
	font.height = 49;
	fonts.insert(std::make_pair(eUITextFont::AlefB36, font));
#endif

#ifdef useQuivira10
	font.width = 11;
	font.height = 14;
	fonts.insert(std::make_pair(eUITextFont::Quivira10, font));
#endif

#ifdef useQuivira14
	font.width = 14;
	font.height = 17;
	fonts.insert(std::make_pair(eUITextFont::Quivira14, font));
#endif

#ifdef useQuivira24
	font.width = 27;
	font.height = 32;
	fonts.insert(std::make_pair(eUITextFont::Quivira24, font));
#endif

#ifdef useQuivira36
	font.width = 40;
	font.height = 49;
	fonts.insert(std::make_pair(eUITextFont::Quivira36, font));
#endif

#ifdef useQuiviraB10
	font.width = 12;
	font.height = 14;
	fonts.insert(std::make_pair(eUITextFont::QuiviraB10, font));
#endif

#ifdef useQuiviraB14
	font.width = 17;
	font.height = 19;
	fonts.insert(std::make_pair(eUITextFont::QuiviraB14, font));
#endif

#ifdef useQuiviraB24
	font.width = 28;
	font.height = 32;
	fonts.insert(std::make_pair(eUITextFont::QuiviraB24, font));
#endif

#ifdef useQuiviraB36
	font.width = 41;
	font.height = 49;
	fonts.insert(std::make_pair(eUITextFont::QuiviraB36, font));
#endif
#ifdef useCourier10
	font.width = 11;
	font.height = 14;
	fonts.insert(std::make_pair(eUITextFont::Courier10, font));
#endif

#ifdef useCourier14
	font.width = 14;
	font.height = 17;
	fonts.insert(std::make_pair(eUITextFont::Courier14, font));
#endif

#ifdef useCourier24
	font.width = 27;
	font.height = 32;
	fonts.insert(std::make_pair(eUITextFont::Courier24, font));
#endif

#ifdef useCourier36
	font.width = 40;
	font.height = 49;
	fonts.insert(std::make_pair(eUITextFont::Courier36, font));
#endif

#ifdef useCourierB10
	font.width = 12;
	font.height = 14;
	fonts.insert(std::make_pair(eUITextFont::CourierB10, font));
#endif

#ifdef useCourierB14
	font.width = 17;
	font.height = 19;
	fonts.insert(std::make_pair(eUITextFont::CourierB14, font));
#endif

#ifdef useCourierB24
	font.width = 28;
	font.height = 32;
	fonts.insert(std::make_pair(eUITextFont::CourierB24, font));
#endif

#ifdef useCourierB36
	font.width = 41;
	font.height = 49;
	fonts.insert(std::make_pair(eUITextFont::CourierB36, font));
#endif

#ifdef useDejaVu18
	font.width = 19;
	font.height = 21;
	fonts.insert(std::make_pair(eUITextFont::DejaVu18, font));
#endif
}

void UIText::update(Adafruit_RA8875 *lcd)
{
	if (!needsUpdate)
		return;
	Rectangle textRect = GetFontRect(buttonText, themeSettings.textFont, themeSettings.textInterlineSpacing, themeSettings.textSpacing);

	// First we need to figure out where the lines will be
	Point center = textRect.center();
	FontHelper::setLCDFont(lcd, themeSettings.textFont);
	uint16_t totalTextSize = (fonts[themeSettings.textFont].height + themeSettings.textInterlineSpacing)*buttonTextLines.size() + 2;

	uint16_t y;
	switch (themeSettings.textAlignmentVertical)
	{
	case(eTextAlignment::Top):
		y = textRect.y1;
		break;
	case(eTextAlignment::Middle):
		y = center.y - (((fonts[themeSettings.textFont].height / 2)*buttonTextLines.size()) +
			((buttonTextLines.size() - 1)*themeSettings.textInterlineSpacing));
		break;
	case(eTextAlignment::Bottom):
		y = textRect.y2 - ((fonts[themeSettings.textFont].height + themeSettings.textInterlineSpacing)*buttonTextLines.size() + 2);
	}
	for (unsigned int i = 0; i < buttonTextLines.size(); i++)
	{
		uint16_t x;
		switch (themeSettings.textAlignmentHoriz)
		{
		case (eTextAlignment::Left):
			x = textRect.x1;
			break;
		case (eTextAlignment::Center):
			x = center.y - (((fonts[themeSettings.textFont].width*buttonTextLines[i].size())*
				themeSettings.textScale) + ((buttonTextLines[i].size() - 1)*themeSettings.textSpacing));
			break;
		case (eTextAlignment::Right):
			x = textRect.y2 - (((fonts[themeSettings.textFont].width*buttonTextLines[i].size())*
				themeSettings.textScale) + ((buttonTextLines[i].size() - 1)*themeSettings.textSpacing));
			break;
		default:
			x = center.y - (((fonts[themeSettings.textFont].width*buttonTextLines[i].size())*
				themeSettings.textScale) + ((buttonTextLines[i].size() - 1)*themeSettings.textSpacing));
			break;
		}
		lcd->setTextColor(themeSettings.textColor);
		lcd->textEnlarge(themeSettings.textScale);
		lcd->setCursor(x, y);
		lcd->textWrite(buttonTextLines[i].c_str());
	}
	needsUpdate = false;
}

void UIText::setText(std::string text)
{
	buttonText = text;
	setButtonLines();
	needsUpdate = true;
}

void UIText::setObjectState(eThemeObjState state)
{
	themeSettings = theme->LoadTheme(eThemeObjType::clickable, state);
}