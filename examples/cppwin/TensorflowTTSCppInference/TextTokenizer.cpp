#include "TextTokenizer.h"
#include "ext/ZCharScanner.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <stdexcept>

// Punctuation, this gets auto-converted to SIL
const std::string punctuation = ",.-;";

// Capitals and lowercases, both having equal indexes
const std::string capitals = "QWERTYUIOPASDFGHJKLZXCVBNM";
const std::string lowercase = "qwertyuiopasdfghjklzxcvbnm";

// Characters that are allowed but don't fit in any other category
const std::string misc = "'";

using namespace std;

void TextTokenizer::SetAllowedChars(const std::string& value)
{
	AllowedChars = value;
}

TextTokenizer::TextTokenizer()
{
}

TextTokenizer::~TextTokenizer()
{
}

vector<string> TextTokenizer::Tokenize(const std::string & InTxt,
	ETTSLanguage::Enum Language)
{
	vector<string> ProcessedTokens;

	ZStringDelimiter Delim(InTxt);
	Delim.AddDelimiter(" ");

	vector<string> DelimitedTokens = Delim.GetTokens();

	// Single word handler
	if (!Delim.szTokens())
		DelimitedTokens.push_back(InTxt);


	// We know that the new vector is going to be at least this size so we reserve
	ProcessedTokens.reserve(DelimitedTokens.size());

	/*
	In this step we go through the string and only allow qualified character to pass through.
	*/
	for (const auto& tok : DelimitedTokens)
	{
		string AppTok = "";
		for (size_t s = 0;s < tok.size();s++)
		{
			// Convert from uppercase to lower
			if (lowercase.find(tok[s]) != string::npos) {
				AppTok += tok[s];
			}

			// No need, because the string is converted before in EnglishCleaner
			//size_t IdxInUpper = capitals.find(tok[s]);
			//if (IdxInUpper != string::npos) {
			//	// Add its lowercase version
			//	AppTok += lowercase[IdxInUpper];
			//}

			// Punctuation handler
			// This time we explicitly add a token to the vector
			if (punctuation.find(tok[s]) != string::npos) {
				// First, if the assembled string isn't empty, we add it in its current state
				// Otherwise, the SIL could end up appearing before the word.
				if (!AppTok.empty()) {
					ProcessedTokens.push_back(AppTok);
					AppTok = "";
				}
				// string punc(1, tok[s]);
				// Add punctuation
				// ProcessedTokens.push_back(punc);
				if (tok[s] != '-') {
					ProcessedTokens.push_back("SIL");
				}
			}

			if (misc.find(tok[s]) != string::npos)
				AppTok += tok[s];

		}
		if (!AppTok.empty())
			ProcessedTokens.push_back(AppTok);

	}
	// Prevent out of range error if the user inputs one word
	if (ProcessedTokens.size() > 1) 
	{
		if (ProcessedTokens[ProcessedTokens.size() - 1] == "SIL")
			ProcessedTokens.pop_back();
	}

	return ProcessedTokens;
}
