#include "phonemizer.h"
#include <fstream>
#include "ext/ZCharScanner.h"

int32_t GetID(const std::vector<IdStr>& In, const std::string& InStr)
{
    for (const IdStr& It : In)
        if (It.STR == InStr)
            return It.ID;

    return -1;
}

std::string GetSTR(const std::vector<IdStr>& In, int32_t InID)
{
    for (const IdStr& It : In)
        if (It.ID == InID)
            return It.STR;

    return "";

}

std::vector<IdStr> Phonemizer::GetDelimitedFile(const std::string& InFname)
{


    std::ifstream InFile(InFname);

    int32_t CuID;
    std::string Tok;
    std::vector<IdStr> RetVec;


    std::string Line;
    while (std::getline(InFile, Line)) {

        if (Line.find("\t") == std::string::npos)
            continue;


        ZStringDelimiter Deline(Line);
        Deline.AddDelimiter("\t");

        CuID = stoi(Deline[1]);
        Tok = Deline[0];


        RetVec.push_back(IdStr{ CuID,Tok });

    }

    return RetVec;


}

void Phonemizer::LoadDictionary(const std::string& InDictFn)
{

    std::ifstream InFile(InDictFn);

    std::string Word;
    std::string Phn;

    if (Dictionary.size())
        Dictionary.clear();

    std::string Line;
    while (std::getline(InFile, Line)) {

        if (Line.find("\t") == std::string::npos)
            continue;

        ZStringDelimiter Deline(Line);
        Deline.AddDelimiter("\t");

        Word = Deline[0];
        Phn = Deline[1];

        Dictionary.push_back(StrStr{ Word,Phn });

    }
    // Sort so lookup can be a bit optimized
    std::sort(Dictionary.begin(), Dictionary.end());

    if (DictBuckets.size())
        DictBuckets.clear();

    size_t LastSize = 0;
    for (size_t i = 0; i < Dictionary.size(); i++)
    {
        const StrStr& Entr = Dictionary[i];

        if (Entr.Word.length() > LastSize)
        {
            LastSize = Entr.Word.length();
            DictBuckets.push_back(VBucket{ LastSize,i });
        }
    }
}

std::string Phonemizer::DictLookup(const std::string& InWord)
{

    for (size_t w = 0; w < Dictionary.size(); w++)
    {
        const StrStr& Entr = Dictionary[w];

        if (Entr.Word.size() != InWord.size())
            continue;

        if (Entr.Word == InWord)
            return Entr.Phn;

    }

    return "";

}

size_t Phonemizer::GetBucketIndex(size_t InSize)
{
    for (const VBucket& Bk : DictBuckets)
    {
        if (Bk.first == InSize)
            return Bk.second;

    }

    return 0;
}

bool Phonemizer::loadHomographDict(const std::string& inHomoDictFn)
{
    std::ifstream InFile(inHomoDictFn);

    std::string word;
    std::string phn;

    if (homographsDict.size())
        homographsDict.clear();

    std::string line;
    while (std::getline(InFile, line)) {

        if (line.find("#") != std::string::npos)
            continue;

        ZStringDelimiter Deline(line);
        Deline.AddDelimiter("|");

        word = Deline[0];
        TextUtils::lowercase(word);
        phn = Deline[1] + "|" + Deline[2] + "|" + Deline[3];

        homographsDict[word] = phn;

    }

    return true;
}


Phonemizer::Phonemizer()
{
    tagger = std::make_unique<PerceptronTagger>();
}

bool Phonemizer::Initialize(const std::string InPath)
{
    // Load indices
    CharId = GetDelimitedFile(InPath + "/char2id.txt");
    PhnId = GetDelimitedFile(InPath + "/phn2id.txt");
    LoadDictionary(InPath + "/dict.txt");
    loadHomographDict(InPath + "/homographs.en");

    // Load tagger files
    tagger->initialize(InPath);

    return true;
}

std::string Phonemizer::ProcessWord(const std::string& InWord, float Temperature)
{
    // Search for homographs in the dict
    auto search = homographsDict.find(InWord);
    if (search != homographsDict.end()) {
        ZStringDelimiter dePhn(search->second);
        dePhn.AddDelimiter("|");
        // if pos startswith dePhn[2]: PhnDict = dePhn[0] else dePhn[1]
        // std::cout << dePhn[0] << " " << dePhn[1] << " " << dePhn[2] << std::endl;
    }

    // First we try dictionary lookup
    // This is because the g2p model can be unreliable, we only want to use it for novel sentences
    std::string PhnDict = DictLookup(InWord);
    if (!PhnDict.empty())
        return PhnDict;
}

std::string Phonemizer::GetPhnLanguage() const
{
    return PhnLanguage;
}

void Phonemizer::SetPhnLanguage(const std::string& value)
{

    PhnLanguage = value;
}

std::string Phonemizer::GetGraphemeChars()
{

    std::string RetAllowed = "";
    for (const IdStr& Idx : CharId)
        RetAllowed.append(Idx.STR);

    return RetAllowed;

}


bool operator<(const StrStr& right, const StrStr& left)
{
    return right.Word.length() < left.Word.length();
}
