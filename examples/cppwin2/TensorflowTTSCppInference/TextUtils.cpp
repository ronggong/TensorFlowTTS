#include "TextUtils.h"
char asciitolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

void TextUtils::lowercase(std::string& text)
{
	std::transform(text.begin(), text.end(), text.begin(), asciitolower);
}

bool TextUtils::isDigits(const std::string& text)
{
	return std::all_of(text.begin(), text.end(), ::isdigit);
}

