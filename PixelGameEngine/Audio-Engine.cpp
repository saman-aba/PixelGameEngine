#include "Audio-Engine.h"

AudioSample::AudioSample()
{

}

AudioSample::AudioSample(std::wstring sWavFile)
{
	FILE* f = nullptr;
	_wfopen_s(&f, sWavFile.c_str(), L"rb");
	if (f == nullptr)
		return;

	char dump[4];
	std::fread(&dump, sizeof(char), 4, f);
	if (strncmp(dump, "RIFF", 4) != 0) 
		return;
	std::fread(&dump, sizeof(char), 4, f);
	std::fread(&dump, sizeof(char), 4, f);
	if ( strncmp(dump, "WAVE", 4) != 0)
		return;

	std::fread(&dump, sizeof(char), 4, f);
	std::fread(&dump, sizeof(char), 4, f);
	std::fread(&wavHeader, sizeof(WAVEFORMATEX) - 2, 1, f);

	if (wavHeader.wBitsPerSample != 16 || wavHeader.nSamplesPerSec != 44100)
	{
		std::fclose(f);
		return;
	}

	long nChunkSize = 0;
	std::fread(&dump, sizeof(char), 4, f);
	std::fread(&nChunkSize, sizeof(long), 1, f);
	while (strncmp(dump, "data", 4) != 0)
	{
		std::fseek(f, nChunkSize, SEEK_CUR);
		std::fread(&dump, sizeof(char), 4, f);
		std::fread(&nChunkSize, sizeof(long), 1, f);
	}
}