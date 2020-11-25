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
	/*for (auto i: vectorid)
		std::cout << i << " ";*/

	TFTensor<float> Mel = MelPredictor.DoInference(vectorid, SpeakerID, Speed, Energy, F0);

	TFTensor<float> AuData = Vocoder.DoInference(Mel);


	int64_t Width = AuData.Shape[0];
	int64_t Height = AuData.Shape[1];
	int64_t Depth = AuData.Shape[2];
	//int z = 0;

	std::vector<float> AudioData;
	AudioData.resize(Height);

	// Code to access 1D array as if it were 3D
	for (int64_t x = 0; x < Width;x++)
	{
		for (int64_t z = 0;z < Depth;z++)
		{
			for (int64_t y = 0; y < Height;y++) {
				int64_t Index = x * Height * Depth + y * Depth + z;
				AudioData[(size_t)y] = AuData.Data[(size_t)Index];

			}

		}
	}


	return AudioData;
}

Voice::~Voice()
{
}
