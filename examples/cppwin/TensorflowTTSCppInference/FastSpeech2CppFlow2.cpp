#include "FastSpeech2CppFlow2.h"

FastSpeech2CppFlow2::FastSpeech2CppFlow2()
{
}

FastSpeech2CppFlow2::FastSpeech2CppFlow2(const std::string& SavedModelFolder)
{
	Initialize(SavedModelFolder);
}


bool FastSpeech2CppFlow2::Initialize(const std::string& SavedModelFolder)
{
	fastspeech = std::make_unique<cppflow::model>(SavedModelFolder);
	return true;
}

cppflow::tensor FastSpeech2CppFlow2::DoInference(const std::vector<int32_t>& InputIDs, int32_t SpeakerID, float Speed, float Energy, float F0)
{
	VX_IF_EXCEPT(!fastspeech, "Tried to do inference on unloaded or invalid model!")

	// Define the tensors
	std::vector<int64_t> InputIDShape = { 1, (int64_t)InputIDs.size() };
	cppflow::tensor input_ids(InputIDs, InputIDShape);
	cppflow::tensor energy_ratios(std::vector<float>{ Energy }, {1});
	cppflow::tensor f0_ratios(std::vector<float>{F0}, {1});
	cppflow::tensor speaker_ids(std::vector<int32_t>{SpeakerID}, {1});
	cppflow::tensor speed_ratios(std::vector<float>{Speed}, {1});

	// Define output tensor
	auto output = (*fastspeech)({ {"serving_default_input_ids", input_ids},
		{"serving_default_energy_ratios", energy_ratios},
		{"serving_default_f0_ratios", f0_ratios},
		{"serving_default_speaker_ids", speaker_ids},
		{"serving_default_speed_ratios", speed_ratios}
		}, { "StatefulPartitionedCall" });

	// We could just straight out define it in the return statement, but I like it more this way
	return output[0];
}

FastSpeech2CppFlow2::~FastSpeech2CppFlow2()
{
}
