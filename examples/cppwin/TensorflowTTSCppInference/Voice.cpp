#include "Voice.h"
#include "ext/ZCharScanner.h"

Voice::Voice(const std::string & VoxPath,
	const std::string& lang)
{
	MelPredictor.Initialize(VoxPath + "/melgen");
	Vocoder.Initialize(VoxPath + "/vocoder");
	processor.init(VoxPath, lang);
}

std::vector<float> Voice::Vocalize(const std::string & Prompt, float Speed, int32_t SpeakerID, float Energy, float F0)
{

	std::vector<int32_t> vectorid = processor.textToSequence(Prompt);
	for (auto i: vectorid)
		std::cout << i << " ";

	Tensor Mel = MelPredictor.DoInference(vectorid, SpeakerID, Speed, Energy, F0);

	Tensor AuData = Vocoder.DoInference(Mel);

	std::vector<float> AudioData = AuData.get_data<float>();

	return AudioData;
}

Voice::~Voice()
{
}
