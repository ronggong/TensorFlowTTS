#pragma once

#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
//#include <algorithm>
#include "TextUtils.h"
#include "EnglishPhoneticProcessor.h"
#include "Phonemizer.h"

class Processor {
private:
    const std::vector<std::string> VALID_SYMBOLS{
        "AA", "AA0", "AA1", "AA2", "AE", "AE0", "AE1", "AE2", "AH", "AH0", "AH1", "AH2", "AO", "AO0", "AO1", "AO2",
        "AW", "AW0", "AW1", "AW2", "AY", "AY0", "AY1", "AY2", "B", "CH", "D", "DH", "EH", "EH0", "EH1", "EH2",
        "ER", "ER0", "ER1", "ER2", "EY", "EY0", "EY1", "EY2", "F", "G", "HH", "IH", "IH0", "IH1", "IH2", 
        "IY", "IY0", "IY1", "IY2", "JH", "K", "L", "M", "N", "NG", "OW", "OW0", "OW1", "OW2", "OY", "OY0", "OY1", "OY2",
        "P", "R", "S", "SH", "T", "TH", "UH", "UH0", "UH1", "UH2",
        "UW", "UW0", "UW1", "UW2", "V", "W", "Y", "Z", "ZH"
    };

    const std::vector<std::string> VALID_SYMBOLS_LIBRITTS{ "AA0","AA1","AA2","AE0","AE1","AE2","AH0","AH1","AH2","AO0","AO1",
        "AO2","AW0","AW1","AW2","AY0","AY1","AY2","B","CH","D","DH","EH0","EH1","EH2","ER0","ER1","ER2","EY0","EY1",
        "EY2","F","G","HH","IH0","IH1","IH2","IY0","IY1","IY2","JH","K","L","M","N","NG","OW0","OW1","OW2","OY0","OY1","OY2",
        "P","R","S","SH","T","TH","UH0","UH1","UH2","UW","UW0","UW1","UW2","V","W","Y","Z","ZH","SIL","END",
        "!", "'", "(", ")", ",", ".", ":", ";", "?", " " };

    const std::string PAD = "_";
    const std::string EOS = "~";
    const std::string PUNCTUATION = "!'(),.:;? ";
    const std::string SPECIAL = "-";
    const std::string LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::vector<std::string> symbols;
    std::unordered_map<std::string, int32_t> SYMBOL_TO_ID;
    std::unordered_map<std::string, std::string> ABBREVIATIONS;

    std::regex COMMA_NUMBER_RE;
    std::regex DECIMAL_NUMBER_RE;
    std::regex DECIMAL_INT_RE;
    std::regex POUNDS_RE;
    std::regex DOLLARS_RE;
    std::regex ORDINAL_RE;
    std::regex NUMBER_RE;

    std::regex WHITESPACE_RE;

    std::string mText;
    void englishCleaners();
    void convertToAscii();
    //void lowercase();
    void expandAbbreviations();
    void expandNumbers();
    void collapseWhitespace();

    void removeCommas();
    void expandDecimals();
    void expandOrdinals();
    void expandCardinals();
    void initSymbolsFS2();
    void initSymbolsLibritts();

    bool libritts;

    EnglishPhoneticProcessor g2p;
    std::vector<std::string> Phonemes;
    std::vector<int32_t> PhonemeIDs;
    std::vector<DictEntry> CurrentDict;

    void ReadPhonemes(const std::string& PhonemePath);

public:
	Processor(bool libritts=true);
	~Processor();
    void init(const std::string& VoxPath, 
        const std::string& lang);
    std::vector<int32_t> textToSequence(const std::string &text);
    void SetDictEntries(const std::vector<DictEntry>& InEntries);
};