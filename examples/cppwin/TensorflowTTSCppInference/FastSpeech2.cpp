#include "FastSpeech2.h"



FastSpeech2::FastSpeech2()
{
}

FastSpeech2::FastSpeech2(const std::string & SavedModelFolder)
{
	Initialize(SavedModelFolder);
}


bool FastSpeech2::Initialize(const std::string & SavedModelFolder)
{
	FastSpeech = std::make_unique<Model>(SavedModelFolder);
	return true;
}

Tensor FastSpeech2::DoInference(const std::vector<int32_t>& InputIDs, int32_t SpeakerID, float Speed, float Energy, float F0)
{
    VX_IF_EXCEPT(!FastSpeech,"Tried to do inference on unloaded or invalid model!")

	// Define the tensors
	Tensor input_ids{ *FastSpeech,"serving_default_input_ids" };
	Tensor energy_ratios{ *FastSpeech,"serving_default_energy_ratios" };
	Tensor f0_ratios{ *FastSpeech,"serving_default_f0_ratios" };
	Tensor speaker_ids{ *FastSpeech,"serving_default_speaker_ids" };
	Tensor speed_ratios{ *FastSpeech,"serving_default_speed_ratios" };

	// This is the shape of the input IDs, our equivalent to tf.expand_dims.
	std::vector<int64_t> InputIDShape = { 1, (int64_t)InputIDs.size() };

	input_ids.set_data(InputIDs, InputIDShape);
	energy_ratios.set_data(std::vector<float>{ Energy });
	f0_ratios.set_data(std::vector<float>{F0});
	speaker_ids.set_data(std::vector<int32_t>{SpeakerID});
	speed_ratios.set_data(std::vector<float>{Speed});

	// Define output tensor
	Tensor output{ *FastSpeech,"StatefulPartitionedCall" };

	// Do inference
	FastSpeech->run({ &input_ids, &speaker_ids, &speed_ratios, &f0_ratios, &energy_ratios }, output);

	// Define output and return it
	// TFTensor<float> Output = VoxUtil::CopyTensor<float>(output);

	return output;
}

FastSpeech2::~FastSpeech2()
{
	/*if (FastSpeech)
		delete FastSpeech;*/
}
