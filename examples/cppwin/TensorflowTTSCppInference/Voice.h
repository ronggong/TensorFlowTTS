#pragma once

#include "FastSpeech2.h"
#include "MultiBandMelGAN.h"
#include "Processor.h"

class Voice
{
private:
	FastSpeech2 MelPredictor;
	MultiBandMelGAN Vocoder;
	Processor processor;

public:
	/* Voice constructor, arguments obligatory.
	 -> VoxPath: Path of folder where models are contained. 
	 --  Must be a folder without an ending slash with UNIX slashes, can be relative or absolute (eg: MyVoices/Karen)
	 --  The folder must contain the following elements:
	 ---  melgen: Folder generated where a FastSpeech2 model was saved as SavedModel, with .pb, variables, etc
	 ---  vocoder: Folder where a Multi-Band MelGAN model was saved as SavedModel.
	 ---  g2p.fst: Phonetisaurus FST G2P model.

	*/
	Voice(const std::string& VoxPath,
		const std::string& lang = "English");

	std::vector<float> Vocalize(const std::string& Prompt, float Speed = 1.f, int32_t SpeakerID = 1, float Energy = 1.f, float F0 = 1.f);


	~Voice();
};

