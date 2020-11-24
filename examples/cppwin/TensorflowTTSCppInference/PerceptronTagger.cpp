#include "PerceptronTagger.h"

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

	if (!parseFromStream(builder, file_weights, &weights, &errs)) {
		std::cout << errs << std::endl;
		return EXIT_FAILURE;
	}
	if (!parseFromStream(builder, file_closed_set, &closed_set, &errs)) {
		std::cout << errs << std::endl;
		return EXIT_FAILURE;
	}
	if (!parseFromStream(builder, file_type_set, &type_set, &errs)) {
		std::cout << errs << std::endl;
		return EXIT_FAILURE;
	}

	return true;
}
