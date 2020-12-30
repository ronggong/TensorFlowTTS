
#include <iostream>
// #include <windows.h>
#include "Voice.h"
#define LOGF(txt) std::cout << txt <<  "\n"

//static std::string ExePath() {
//	char buffer[MAX_PATH];
//	GetModuleFileNameA(NULL, buffer, MAX_PATH);
//	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
//	return std::string(buffer).substr(0, pos);
//}

int main()
{
	 //char filename[] = "LJ";
	 //char fullFilename[MAX_PATH];
	 //GetFullPathNameA(filename, MAX_PATH, fullFilename, nullptr);
	 //std::string exePath = ExePath();
	 //LOGF(exePath);
	
	bool Running = true;
	LOGF("Loading voice...");
	Voice LJSpeech("LJ", "English");
	while (Running) 
	{
		std::string Prompt = "";

		LOGF("Type a prompt, or type EXIT to exit ");

		getline(std::cin, Prompt);
		if (Prompt == "EXIT") {
			Running = false;
			break;
		}
		std::vector<float> Audata = LJSpeech.Vocalize(Prompt);
		
		std::string Filename = Prompt.substr(0, std::min(16, (int)Prompt.size())) + ".wav";

		VoxUtil::ExportWAV(Filename, Audata, 24000);
		LOGF("Saved to " + Filename);
		
	}


	std::cout << "Hello TensorflowTTS!\n";
	return 0;

}
