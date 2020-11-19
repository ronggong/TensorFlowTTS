#pragma once
#include <fstream>
#include "TextTokenizer.h"
#include "Phonemizer.h"
#include "VoxCommon.hpp"

struct DictEntry {
	std::string Word;
	std::string PhSpelling;
};

// Check if the base word is equal to this string
bool operator==(const DictEntry& left, const std::string& right);

class EnglishPhoneticProcessor
{
private:
	TextTokenizer Tokenizer;
	Phonemizer* Phoner;

	inline bool FileExists(const std::string& name) {
		std::ifstream f(name.c_str());
		return f.good();
	}

public:
	bool Initialize(Phonemizer* InPhn);
	std::string ProcessTextPhonetic(const std::string& InText, 
		const std::vector<std::string>& InPhonemes, 
		const std::vector<DictEntry>& InDict, 
		ETTSLanguage::Enum InLanguage);
	EnglishPhoneticProcessor();
	EnglishPhoneticProcessor(Phonemizer* InPhn);
	~EnglishPhoneticProcessor();
};

