#include "EnglishPhoneticProcessor.h"

using namespace std;

bool EnglishPhoneticProcessor::Initialize(Phonemizer* InPhn, const std::string& path)
{
	Phoner = InPhn;
	Tokenizer.SetAllowedChars(Phoner->GetGraphemeChars());

	// Load tagger files
	tagger->initialize(path);

	g2pseq->initialize(path);

	return true;
}

std::string EnglishPhoneticProcessor::ProcessTextPhonetic(const std::string& InText, 
	const std::vector<string>& InPhonemes, 
	const std::vector<DictEntry>& InDict, 
	ETTSLanguage::Enum InLanguage)
{
	if (!Phoner)
		return "ERROR";

	vector<string> Words = Tokenizer.Tokenize(InText, InLanguage);

	// Detect Homographs in words
	// homoPhns: ["pron1|pron2|pos", ...]
	// homoDetected == true if detected homograph in Words
	vector<string> homoPhns;
	bool homoDetected = Phoner->detectHomograph(Words, homoPhns);

	std::vector<std::pair<std::string, std::string>> taggedWords;
	if (homoDetected) {
		taggedWords = tagger->tag(Words);
	}

	string Assemble = "";
	// Make a copy of the dict passed.
	std::vector<DictEntry> CurrentDict = InDict;

	for (size_t w = 0; w < Words.size(); w++)

	{
		const string& Word = Words[w];

		// SIL goes directly to the output
		if (Word == "SIL") {
			Assemble.append(Word);
			Assemble.append(" ");
			continue;
		}

		if (Word.find("@") != std::string::npos) {
			std::string AddPh = Word.substr(1); // Remove the @
			size_t OutId = 0;
			if (VoxUtil::FindInVec(AddPh, InPhonemes, OutId))
			{
				Assemble.append(InPhonemes[OutId]);
				Assemble.append(" ");
			}
			continue;
		}

		size_t OverrideIdx = 0;
		if (VoxUtil::FindInVec2<std::string, DictEntry>(Word, InDict, OverrideIdx))
		{
			Assemble.append(InDict[OverrideIdx].PhSpelling);
			Assemble.append(" ");
			continue;
		}

		// Detected homographs
		if (homoDetected && homoPhns[w].size() > 0) {
			vector<string> pronsVec;
			Phoner->separateProns(homoPhns[w], pronsVec);
			// if pos starts with pronsVec[2] == "V"
			if (taggedWords[w].second.rfind(pronsVec[2], 0) == 0) {
				Assemble.append(pronsVec[0]);
			}
			else {
				Assemble.append(pronsVec[1]);
			}
			Assemble.append(" ");
			continue;
		}

		std::string Res = Phoner->ProcessWord(Word, 0.001f);

		if (Res.empty()) {
			Res = g2pseq->predict(Word);
		}

		// Cache the word in the override dict so next time we don't have to research it
		CurrentDict.push_back({ Word, Res });

		Assemble.append(Res);
		Assemble.append(" ");

	}


	// Delete last space if there is
	if (Assemble[Assemble.size() - 1] == ' ')
		Assemble.pop_back();


	return Assemble;
}

EnglishPhoneticProcessor::EnglishPhoneticProcessor()
{
	tagger = std::make_unique<PerceptronTagger>();
	g2pseq = std::make_unique<G2pseq>();
	Phoner = nullptr;
}

EnglishPhoneticProcessor::EnglishPhoneticProcessor(Phonemizer* InPhn, const std::string& path)
{
	Initialize(InPhn, path);
}


EnglishPhoneticProcessor::~EnglishPhoneticProcessor()
{
	if (Phoner)
		delete Phoner;
}

bool operator==(const DictEntry& left, const std::string& right)
{
	return left.Word == right;
}