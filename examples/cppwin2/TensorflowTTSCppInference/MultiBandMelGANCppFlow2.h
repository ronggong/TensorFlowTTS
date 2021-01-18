#pragma once

#include "ext/CppFlow2/cppflow.h"
#include "VoxCommon.hpp"
class MultiBandMelGANCppFlow2
{
private:
	//Model* MelGAN;
	std::unique_ptr<cppflow::model> melgan;

public:
	bool Initialize(const std::string& VocoderPath);


	// Do MultiBand MelGAN inference including PQMF
	// -> InMel:  Mel spectrogram (shape [1, xx, 80])
	// <- Returns: Tensor data [4, xx, 1]
	cppflow::tensor DoInference(const cppflow::tensor& InMel);

	MultiBandMelGANCppFlow2();
	~MultiBandMelGANCppFlow2();
};