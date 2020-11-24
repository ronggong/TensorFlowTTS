#pragma once
#include <fstream>
#include <iostream>
#include "json/json.h"

class PerceptronTagger
{
private:
	Json::Value weights;
	Json::Value closed_set;
	Json::Value type_set;
public:
	PerceptronTagger();
	~PerceptronTagger();
	bool initialize(std::string path);
};