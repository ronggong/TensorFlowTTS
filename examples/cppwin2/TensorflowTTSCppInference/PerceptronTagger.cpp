#include "PerceptronTagger.h"

void add_features(std::unordered_map<std::string, int>& features,
	const std::string& name,
	const std::string& arg) {
	std::string key = name + " " + arg;
	auto search = features.find(key);
	if (search != features.end()) {
		features[key]++;
	}
	else {
		features[key] = 1;
	}
}

void add_features(std::unordered_map<std::string, int>& features,
	const std::string& name,
	const std::string& arg0,
	const std::string& arg1) {
	std::string key = name + " " + arg0 + " " + arg1;
	auto search = features.find(key);
	if (search != features.end()) {
		features[key]++;
	}
	else {
		features[key] = 1;
	}
}

std::string PerceptronTagger::normalize(const std::string& word)
{
	//Normalization used in pre - processing.
	//	- All words are lower cased
	//	- Groups of digits of length 4 are represented as !YEAR;
	//-Other digits are represented as !DIGITS
	//	:rtype: str

	if (word.find("-") != std::string::npos && word.at(0) != '-') {
		return "!HYPHEN";
	}
	if (word.size() == 4 && TextUtils::isDigits(word)) {
		return "!YEAR";
	}
	if (word.size() > 0 && TextUtils::isDigits(word.substr(0, 1))) {
		return "!DIGITS";
	}
	return word;
}

std::unordered_map<std::string, int> PerceptronTagger::get_features(size_t i, 
	const std::string& word, 
	const std::vector<std::string>& context, 
	const std::string prev, 
	const std::string prev2)
{
	std::unordered_map<std::string, int> features;
	i += START.size();
	features["bias"] = 1;
	size_t pos = (static_cast<int>(word.size()) - 3 > 0) ? (static_cast<int>(word.size()) - 3) : 0;
	add_features(features, "i suffix", word.substr(pos));
	add_features(features, "i pref1", word.substr(0, 1));
	add_features(features, "i-1 tag", prev);
	add_features(features, "i-2 tag", prev2);
	add_features(features, "i tag+i-2 tag", prev, prev2);
	add_features(features, "i word", context[i]);
	add_features(features, "i-1 tag+i word", prev, context[i]);
	add_features(features, "i-1 word", context[i - 1]);
	pos = (static_cast<int>(context[i - 1].size()) - 3 > 0) ? static_cast<int>(context[i-1].size()) - 3 : 0;
	add_features(features, "i-1 suffix", context[i - 1].substr(pos));
	add_features(features, "i-2 word", context[i - 2]);
	add_features(features, "i+1 word", context[i + 1]);
	pos = (static_cast<int>(context[i + 1].size()) - 3 > 0) ? static_cast<int>(context[i + 1].size()) - 3 : 0;
	add_features(features, "i+1 suffix", context[i + 1].substr(pos));
	add_features(features, "i+2 word", context[i + 2]);
	return features;
}

std::string PerceptronTagger::predict(std::unordered_map<std::string, int>& features)
{
	// """Dot-product the features and current weights and return the best label."""
	std::unordered_map<std::string, float> scores;
	std::string tag;

	for (auto& f : features) {
		float value = static_cast<float>(f.second);
		if (!weights.isMember(f.first) || value == 0) {
			continue;
		}
		Json::Value w_feature = weights[f.first];
		// members are keys
		std::vector<std::string> memberNames = w_feature.getMemberNames();
		// iterate through keys
		for (const std::string& label: memberNames) {
			float weight = w_feature[label].asFloat();
			auto search = scores.find(label);
			if (search != scores.end()) {
				scores[label] += value * weight;
			}
			else {
				scores[label] = value * weight;
			}
		}
	}
	
	float max_value = std::numeric_limits<float>::min();
	char max_char = 'a';
	for (auto const& pair : scores) {
		// check label is in classes
		if (classes.find(pair.first) == classes.end()) {
			continue;
		}
		// Get the largest tag by comparing the value
		if (pair.second > max_value) {
			tag = pair.first;
			max_value = pair.second;
			max_char = pair.first.at(0);
		}
		else if (pair.second == max_value && pair.first.at(0) > max_char) {
			// second sort to compare the first char
			tag = pair.first;
			max_char = pair.first.at(0);
		}
	}
	return tag;
}

PerceptronTagger::PerceptronTagger()
{
}

PerceptronTagger::~PerceptronTagger()
{
}

bool PerceptronTagger::initialize(std::string path)
{
	std::ifstream file_weights(path + "/tagger/weights.json");
	std::ifstream file_closed_set(path + "/tagger/closed_set.json");
	std::ifstream file_type_set(path + "/tagger/type_set.json");

	Json::CharReaderBuilder builder;
	builder["collectComments"] = true;
	JSONCPP_STRING errs;

	Json::Value classesTmp;

	if (!parseFromStream(builder, file_weights, &weights, &errs)) {
		std::cout << errs << std::endl;
		return EXIT_FAILURE;
	}
	if (!parseFromStream(builder, file_closed_set, &tagdict, &errs)) {
		std::cout << errs << std::endl;
		return EXIT_FAILURE;
	}
	if (!parseFromStream(builder, file_type_set, &classesTmp, &errs)) {
		std::cout << errs << std::endl;
		return EXIT_FAILURE;
	}

	for (size_t i = 0; i < classesTmp.size(); i++) {
		classes.insert(classesTmp[int(i)].asString());
	}

	return true;
}

std::vector<std::pair<std::string, std::string>> PerceptronTagger::tag(std::vector<std::string>& tokens)
{
	std::string prev = START[0];
	std::string prev2 = START[1];

	std::string tagStr;
	std::unordered_map<std::string, int> features;
	std::vector<std::pair<std::string, std::string>> output;

	std::vector<std::string> context;
	context.insert(context.end(), START.begin(), START.end());
	for (size_t i = 0; i < tokens.size(); i++) {
		std::string normedToken = normalize(tokens[i]);
		// std::cout << normedToken << std::endl;
		context.push_back(normedToken);
	}
	context.insert(context.end(), END.begin(), END.end());
	for (size_t i = 0; i < tokens.size(); i++) {
		if (tagdict.isMember(tokens[i])) {
			tagStr = tagdict[tokens[i]].asString();
		}
		else {
			features = get_features(i, tokens[i], context, prev, prev2);
			// std::cout << tokens[i] << std::endl;
			/*for (auto const& pair : features) {
				std::cout << "{" << pair.first << ": " << pair.second << "}\n";
			}*/
			tagStr = predict(features);

		}
		output.push_back(std::make_pair( tokens[i], tagStr ));
		prev2 = prev;
		prev = tagStr;
	}
	
	return output;
}
