#include "MultiBandMelGAN.h"



bool MultiBandMelGAN::Initialize(const std::string & VocoderPath)
{
	MelGAN = std::make_unique<Model>(VocoderPath);
	return true;

}

Tensor MultiBandMelGAN::DoInference(Tensor& InMel)
{
    VX_IF_EXCEPT(!MelGAN, "Tried to infer MB-MelGAN on uninitialized model!!!!");

	// Convenience reference so that we don't have to constantly derefer pointers.
	Tensor input_mels{ *MelGAN,"serving_default_mels" };
	input_mels.set_data(InMel.get_data<float>(), InMel.get_shape());

	Tensor out_audio{ *MelGAN, "StatefulPartitionedCall" };

	MelGAN->run(input_mels, out_audio);

	// TFTensor<float> RetTensor = VoxUtil::CopyTensor<float>(out_audio);

	return out_audio;


}

MultiBandMelGAN::MultiBandMelGAN()
{
	// MelGAN = nullptr;
}


MultiBandMelGAN::~MultiBandMelGAN()
{
}
