#pragma once
#include <vector>
#include <string>
#include "VoxCommon.hpp"

class TextTokenizer
{
private:
	std::string AllowedChars;

public:
	TextTokenizer();
	~TextTokenizer();

	std::vector<std::string> Tokenize(const std::string& InTxt,
		ETTSLanguage::Enum Language = ETTSLanguage::Enum::English);
	void SetAllowedChars(const std::string& value);
};

