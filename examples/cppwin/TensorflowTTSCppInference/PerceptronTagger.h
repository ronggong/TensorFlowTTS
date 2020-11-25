#pragma once
#include <set>
#include <limits>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "TextUtils.h"
#include "json/json.h"

// Implementation inference of PerceptronTagger in nltk

class PerceptronTagger
{
private:
	Json::Value weights;
	Json::Value tagdict;
	// Json::Value classes;
	std::set<std::string> classes;

	std::vector<std::string> START{ "-START-", "-START2-" };
	std::vector<std::string> END{ "-END-", "-END2-" };

	std::string normalize(const std::string& word);
	std::unordered_map<std::string, int> get_features(size_t i, const std::string& word,
		const std::vector<std::string>& context,
		const std::string prev,
		const std::string prev2);
	std::string predict(std::unordered_map<std::string, int>& features);

public:
	PerceptronTagger();
	~PerceptronTagger();
	bool initialize(std::string path);
	std::vector<std::pair<std::string, std::string>> tag(std::vector<std::string>& tokens);
};