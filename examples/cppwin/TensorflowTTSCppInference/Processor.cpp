#include <algorithm>
#include <iostream>
#include "Processor.h"

const std::vector<std::string> first14 = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen", "fourteen" };
const std::vector<std::string> prefixes = { "twen", "thir", "for", "fif", "six", "seven", "eigh", "nine" };

std::string IntToStr(int number)
{
	if (number < 0)
	{
		return "minus " + IntToStr(-number);
	}
	if (number <= 14)
		return first14.at(number);
	if (number < 20)
		return prefixes.at(number - 12) + "teen";
	if (number < 100) {
		unsigned int remainder = number - (static_cast<int>(number / 10) * 10);
		return prefixes.at(number / 10 - 2) + (0 != remainder ? "ty " + IntToStr(remainder) : "ty");
	}
	if (number < 1000) {
		unsigned int remainder = number - (static_cast<int>(number / 100) * 100);
		return first14.at(number / 100) + (0 != remainder ? " hundred " + IntToStr(remainder) : " hundred");
	}
	if (number < 1000000) {
		unsigned int thousands = static_cast<int>(number / 1000);
		unsigned int remainder = number - (thousands * 1000);
		return IntToStr(thousands) + (0 != remainder ? " thousand " + IntToStr(remainder) : " thousand");
	}
	if (number < 1000000000) {
		unsigned int millions = static_cast<int>(number / 1000000);
		unsigned int remainder = number - (millions * 1000000);
		return IntToStr(millions) + (0 != remainder ? " million " + IntToStr(remainder) : " million");
	}
	throw std::out_of_range("inttostr() value too large");
}

void Processor::englishCleaners()
{
	// convertToAscii();
	// std::cout << mText << std::endl;

	lowercase();

	expandNumbers();

	collapseWhitespace();

}

void Processor::convertToAscii()
{
}

char asciitolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

void Processor::lowercase()
{
	std::transform(mText.begin(), mText.end(), mText.begin(), asciitolower);
}

void Processor::expandAbbreviations()
{
}

void Processor::expandNumbers()
{
	removeCommas();
	expandDecimals();
	std::cout << "expandNumbers: " << mText << std::endl;

	expandCardinals();
	std::cout << "expandNumbers: " << mText << std::endl;

}

void Processor::collapseWhitespace()
{
	mText = std::regex_replace(mText, WHITESPACE_RE, " ");
}

void Processor::removeCommas()
{
	std::sregex_iterator iter(mText.begin(), mText.end(), COMMA_NUMBER_RE);
	std::sregex_iterator end;
	std::vector<std::string> strFounds;
	std::string strReplace;
	while (iter != end) {
		for (size_t i = 0; i < iter->size(); ++i) {
			strFounds.push_back((*iter)[i].str());
		}
		++iter;
	}
	for (size_t i = 0; i < strFounds.size(); i++) {
		strReplace = std::regex_replace(strFounds[i], std::regex(","), "");
		mText = std::regex_replace(mText, std::regex(strFounds[i]), strReplace, std::regex_constants::format_first_only);
	}
}

void Processor::expandDecimals()
{
	std::sregex_iterator iter(mText.begin(), mText.end(), DECIMAL_NUMBER_RE);
	std::sregex_iterator end;
	std::vector<std::string> strFounds;
	std::string strReplace;

	while (iter != end) {
		for (size_t i = 0; i < iter->size(); ++i) {
			strFounds.push_back((*iter)[i].str());
		}
		++iter;
	}

	for (size_t i = 0; i < strFounds.size(); i++) {
		strReplace = std::regex_replace(strFounds[i], std::regex("\\."), " point ");
		mText = std::regex_replace(mText, std::regex(strFounds[i]), strReplace, std::regex_constants::format_first_only);
	}
}

void Processor::expandCardinals()
{
	std::sregex_iterator iter(mText.begin(), mText.end(), NUMBER_RE);
	std::sregex_iterator end;
	std::vector<std::string> strFounds;

	while (iter != end) {
		for (size_t i = 0; i < iter->size(); ++i) {
			strFounds.push_back((*iter)[i].str());
		}
		++iter;
	}

	for (size_t i = 0; i < strFounds.size(); i++) {
		mText = std::regex_replace(mText, std::regex(strFounds[i]), IntToStr(stoi(strFounds[i])), std::regex_constants::format_first_only);
	}
}

Processor::Processor()
{
	COMMA_NUMBER_RE = "[0-9]+\\,+[0-9]+";
	DECIMAL_NUMBER_RE = "[0-9]+\\.[0-9]+";
	POUNDS_RE = "£([0-9,]*[0-9]+)";
	DOLLARS_RE = "\\$([0-9\\.\\,]*[0-9]+)";
	ORDINAL_RE = "[0-9]+(st|nd|rd|th)";
	NUMBER_RE = "[0-9]+";
	
	WHITESPACE_RE = "\\s+";

	for (size_t i = 0; i < PAD.size(); i++) {
		symbols.push_back(PAD.substr(i, 1));
	}
	for (size_t i = 0; i < SPECIAL.size(); i++) {
		symbols.push_back(SPECIAL.substr(i, 1));
	}
	for (size_t i = 0; i < PUNCTUATION.size(); i++) {
		symbols.push_back(PUNCTUATION.substr(i, 1));
	}
	for (size_t i = 0; i < LETTERS.size(); i++) {
		symbols.push_back(LETTERS.substr(i, 1));
	}
	for (size_t i = 0; i < VALID_SYMBOLS.size(); i++) {
		symbols.push_back("@"+VALID_SYMBOLS[i]);
	}
	for (size_t i = 0; i < EOS.size(); i++) {
		symbols.push_back(EOS.substr(i, 1));
	}
	for (int32_t i = 0; i < symbols.size(); ++i) {
		SYMBOL_TO_ID[symbols[i]] = i;
	}
}

Processor::~Processor()
{
}

std::vector<int32_t> Processor::textToSequence(const std::string &text)
{
	mText = text;
	std::cout << mText << std::endl;

	std::vector<int32_t> sequence;
	if (!mText.empty()) {
		englishCleaners();
		std::string textSubstr;
		for (size_t i = 0; i < mText.size(); i++) {
			textSubstr = mText.substr(i, 1);
			if (textSubstr != "_" && textSubstr != "~" && std::find(symbols.begin(), symbols.end(), textSubstr) != symbols.end()) {
				sequence.push_back(SYMBOL_TO_ID[textSubstr]);
			}
		}
	}
	return sequence;
}
