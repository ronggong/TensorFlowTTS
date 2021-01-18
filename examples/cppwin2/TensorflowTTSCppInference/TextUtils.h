#pragma once
#include <string>
#include <algorithm>

const std::string PUNCT = "!'(),.:;?";

class TextUtils {
public:
	static void lowercase(std::string& text);
	static bool isDigits(const std::string& text);
};