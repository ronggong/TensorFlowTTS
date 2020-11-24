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
