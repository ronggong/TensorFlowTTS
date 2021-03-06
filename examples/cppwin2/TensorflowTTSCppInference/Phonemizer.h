#pragma once
#ifndef PHONEMIZER_H
#define PHONEMIZER_H
#include <vector>
#include <iostream>
#include <string>
#include <tuple>
#include <set>
#include <algorithm>
#include <unordered_map>
#include "TextUtils.h"

struct IdStr {
    int32_t ID;
    std::string STR;
};


struct StrStr {
    std::string Word;
    std::string Phn;
};

// Length, start index in vec
typedef std::pair<size_t, size_t> VBucket;

class Phonemizer
{
private:

    std::vector<IdStr> CharId;
    std::vector<IdStr> PhnId;
    std::vector<VBucket> DictBuckets;
    std::unordered_map<std::string, std::string> homographsDict;

    std::vector<IdStr> GetDelimitedFile(const std::string& InFname);

    // Sorry, can't use set, unordered_map or any other types. (I tried)
    std::vector<StrStr> Dictionary;

    void LoadDictionary(const std::string& InDictFn);

    std::string DictLookup(const std::string& InWord);

    size_t GetBucketIndex(size_t InSize);

    bool loadHomographDict(const std::string& inHomoDictFn);

    std::string PhnLanguage;
public:
    Phonemizer();
    /*
     * Initialize a phonemizer
     * Expects:
     * - Two files consisting in TOKEN \t ID:
     * -- char2id.txt: Translation from input character to ID the model can accept
     * -- phn2id.txt: Translation from output ID from the model to phoneme
     * - A model/ folder where a G2P-Tensorflow model was saved as SavedModel
     * - dict.txt: Phonetic dictionary. First it searches the word there and if it can't be found then it uses the model.

    */
    bool Initialize(const std::string InPath);
    bool detectHomograph(const std::vector<std::string>& words,
        std::vector<std::string>& phns);
    void separateProns(const std::string& prons,
        std::vector<std::string>& pronsVec);
    std::string ProcessWord(const std::string& InWord,
        float Temperature = 0.1f);
    std::string GetPhnLanguage() const;
    void SetPhnLanguage(const std::string& value);

    std::string GetGraphemeChars();
};

bool operator<(const StrStr& right, const StrStr& left);
#endif // PHONEMIZER_H
