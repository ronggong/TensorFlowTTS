#include <iostream>
#include <sstream>
#include <iterator>
#include "Processor.h"
#include "ext/ZCharScanner.h"

const std::vector<std::string> nums{
	"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
	"ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"
};

const std::vector<std::string> tens{ "zero", "ten", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety" };

std::unordered_map<std::string, std::string> ORDINAL_MAP = {
		{"one", "first"},
		{"two", "second"},
		{"three", "third"},
		{"five", "fifth"},
		{"eight", "eighth"},
		{"nine", "ninth"},
		{"twelve", "twelfth"},
};

template <typename Out>
void splitString(const std::string& s, char delim, Out result) {
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim)) {
		if (!item.empty())
			*result++ = item;
	}
}

std::vector<std::string> splitString(const std::string& s, char delim) {
	std::vector<std::string> elems;
	splitString(s, delim, std::back_inserter(elems));
	return elems;
}

std::string numToStringHelper(long long n) {
	if (n < 0) {
		return "negative " + numToStringHelper(-n);
	}
	long long index = n;
	if (n <= 19) {
		return nums[index];
	}
	if (n <= 99) {
		return tens[index / 10] + (n % 10 > 0 ? "-" + numToStringHelper(n % 10) : "");
	}
	std::string label;
	long long factor = 0;
	if (n <= 999) {
		label = "hundred";
		factor = 100;
	}
	else if (n <= 999999) {
		label = "thousand";
		factor = 1000;
	}
	else if (n <= 999999999) {
		label = "million";
		factor = 1000000;
	}
	else if (n <= 999999999999) {
		label = "billion";
		factor = 1000000000;
	}
	else if (n <= 999999999999999) {
		label = "trillion";
		factor = 1000000000000;
	}
	else if (n <= 999999999999999999) {
		label = "quadrillion";
		factor = 1000000000000000;
	}
	else {
		label = "quintillion";
		factor = 1000000000000000000;
	}
	return numToStringHelper(n / factor) + " " + label + (n % factor > 0 ? " " + numToStringHelper(n % factor) : "");
}

std::string numToString(long long n) {
	return numToStringHelper(n);
}

std::string toOrdinal(long long n) {
	std::string spelling = numToString(n);
	// Split the spelling by space
	std::istringstream iss(spelling);
	std::vector<std::string> split{ std::istream_iterator<std::string>{iss},
									std::istream_iterator<std::string>{} };
	std::string last = split[split.size() - 1];
	std::string replace;
	if (last.find("-") != std::string::npos) {
		std::vector<std::string> lastSplit = splitString(last, '-');
		std::string lastWithDash = lastSplit[1];
		std::string lastReplace;
		if (ORDINAL_MAP.find(lastWithDash) != ORDINAL_MAP.end()) {
			lastReplace = ORDINAL_MAP[lastWithDash];
		}
		else if (lastWithDash.back() == 'y') {
			lastReplace = lastWithDash.substr(0, lastWithDash.size() - 1) + "ieth";
		}
		else {
			lastReplace = lastWithDash + "th";
		}
		replace = lastSplit[0] + "-" + lastReplace;
	}
	else {
		if (ORDINAL_MAP.find(last) != ORDINAL_MAP.end()) {
			replace = ORDINAL_MAP[last];
		}
		else if (last.back() == 'y') {
			replace = last.substr(0, last.size() - 1) + "ieth";
		}
		else {
			replace = last + "th";
		}
	}
	split[split.size() - 1] = replace;
	const char* const delim = " ";
	std::ostringstream joined;
	std::copy(split.begin(), split.end(), std::ostream_iterator<std::string>(joined, delim));
	return joined.str();
}

void Processor::englishCleaners()
{
	// convertToAscii();

	lowercase();
	expandAbbreviations();
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
	for (auto& it : ABBREVIATIONS) {
		mText = std::regex_replace(mText, std::regex("\\b" + it.first + "\\."), it.second);
	}
}

void Processor::expandNumbers()
{
	removeCommas();
	expandDecimals();
	expandOrdinals();
	expandCardinals();

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
		std::smatch decimal;
		std::string decimalSpace;
		// Find the decimal after point
		std::regex_search(strFounds[i], decimal, DECIMAL_INT_RE);
		for (auto c : decimal.str()) {
			decimalSpace += c;
			decimalSpace += " ";
		}
		strReplace = std::regex_replace(strFounds[i], std::regex(decimal.str()), decimalSpace);
		strReplace = std::regex_replace(strReplace, std::regex("\\."), " point", std::regex_constants::format_first_only);
		mText = std::regex_replace(mText, std::regex(strFounds[i]), strReplace, std::regex_constants::format_first_only);
	}
}

void Processor::expandOrdinals()
{
	std::sregex_iterator iter(mText.begin(), mText.end(), ORDINAL_RE);
	std::sregex_iterator end;
	std::vector<std::string> strFounds;

	while (iter != end) {
		for (size_t i = 0; i < iter->size(); ++i) {
			strFounds.push_back((*iter)[i].str());
		}
		++iter;
	}

	for (size_t i = 0; i < strFounds.size(); i++) {
		if (strFounds[i].size() == 2) {
			continue;
		}
		std::string s = strFounds[i].substr(0, strFounds[i].size() - 2);
		mText = std::regex_replace(mText, std::regex(strFounds[i]), toOrdinal(stoll(s)), std::regex_constants::format_first_only);
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
		mText = std::regex_replace(mText, std::regex(strFounds[i]), numToString(stoll(strFounds[i])), std::regex_constants::format_first_only);
	}
}

void Processor::initSymbolsFS2()
{
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
		symbols.push_back("@" + VALID_SYMBOLS[i]);
	}
	for (size_t i = 0; i < EOS.size(); i++) {
		symbols.push_back(EOS.substr(i, 1));
	}
	for (size_t i = 0; i < symbols.size(); ++i) {
		SYMBOL_TO_ID[symbols[i]] = int(i);
	}
}

void Processor::initSymbolsLibritts()
{
	for (size_t i = 0; i < VALID_SYMBOLS_LIBRITTS.size(); i++) {
		symbols.push_back(VALID_SYMBOLS_LIBRITTS[i]);
	}
	for (int32_t i = 0; i < symbols.size(); i++) {
		SYMBOL_TO_ID[symbols[i]] = i + 4;
	}
}

void Processor::init(const std::string& VoxPath,
	const std::string& lang)
{
	Phonemizer* phn = new Phonemizer;
	phn->Initialize("g2p/" + lang);
	phn->SetPhnLanguage(lang);
	g2p.Initialize(phn);
	ReadPhonemes(VoxPath + "/phonemes.txt");
}

Processor::Processor(bool libritts)
	: libritts (libritts)
{
	COMMA_NUMBER_RE = "[0-9]+\\,+[0-9]+";
	DECIMAL_NUMBER_RE = "[0-9]+\\.[0-9]+";
	DECIMAL_INT_RE = "\\.[0-9]+";
	POUNDS_RE = "£([0-9,]*[0-9]+)";
	DOLLARS_RE = "\\$([0-9\\.\\,]*[0-9]+)";
	ORDINAL_RE = "[0-9]+(st|nd|rd|th)";
	NUMBER_RE = "[0-9]+";
	WHITESPACE_RE = "\\s+";

	ABBREVIATIONS = {
		{"mrs", "misess"},
		{"mr", "mister" },
		{"dr", "doctor"},
		{"st", "saint" },
		{"co", "company"},
		{"jr", "junior"},
		{"maj", "major"},
		{"gen", "general"},
		{"drs", "doctors"},
		{"rev", "reverend"},
		{"lt", "lieutenant"},
		{"hon", "honorable"},
		{"sgt", "sergeant"},
		{"capt", "captain"},
		{"esq", "esquire"},
		{"ltd", "limited"},
		{"col", "colonel"},
		{"ft", "fort"},
	};

	if (libritts) {
		initSymbolsLibritts();
	}
	else {
		initSymbolsFS2();
	}
}

Processor::~Processor()
{
}

void Processor::ReadPhonemes(const std::string& PhonemePath)
{
	std::ifstream Phone(PhonemePath);

	std::string Line;
	while (std::getline(Phone, Line))
	{
		if (Line.find("\t") == std::string::npos)
			continue;


		ZStringDelimiter Deline(Line);
		Deline.AddDelimiter("\t");

		Phonemes.push_back(Deline[0]);
		PhonemeIDs.push_back(std::stoi(Deline[1]));
	}
}

void Processor::SetDictEntries(const std::vector<DictEntry>& InEntries)
{
	CurrentDict = InEntries;
}

std::vector<int32_t> Processor::textToSequence(const std::string &text)
{
	mText = text;

	std::vector<int32_t> sequence;
	if (!mText.empty()) {
		englishCleaners();
		if (libritts) {
			mText = g2p.ProcessTextPhonetic(mText, Phonemes, CurrentDict, ETTSLanguage::Enum::English);
			std::vector<std::string> textSplit = splitString(mText, ' ');
			if (textSplit[textSplit.size() - 1] != "SIL") {
				textSplit.push_back("SIL");
			}
			textSplit.push_back("END");
			for (size_t i = 0; i < textSplit.size(); i++) {
				std::cout << textSplit[i] << std::endl;
				sequence.push_back(SYMBOL_TO_ID[textSplit[i]]);
			}
		}
		else {
			std::string textSubstr;
			for (size_t i = 0; i < mText.size(); i++) {
				textSubstr = mText.substr(i, 1);
				if (textSubstr != "_" && textSubstr != "~" && std::find(symbols.begin(), symbols.end(), textSubstr) != symbols.end()) {
					sequence.push_back(SYMBOL_TO_ID[textSubstr]);
				}
			}
		}
	}
	return sequence;
}
