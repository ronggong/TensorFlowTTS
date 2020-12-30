#include "MultiBandMelGANCppFlow2.h"


bool MultiBandMelGANCppFlow2::Initialize(const std::string& VocoderPath)
{
	melgan = std::make_unique<cppflow::model>(VocoderPath);
	return true;
}

cppflow::tensor MultiBandMelGANCppFlow2::DoInference(const cppflow::tensor& InMel)
{
	VX_IF_EXCEPT(!melgan, "Tried to infer MB-MelGAN on uninitialized model!!!!");

	auto output = (*melgan)({ { "serving_default_mels", InMel } }, { "StatefulPartitionedCall" });

	return output[0];
}

MultiBandMelGANCppFlow2::MultiBandMelGANCppFlow2()
{
}


MultiBandMelGANCppFlow2::~MultiBandMelGANCppFlow2()
{
}
