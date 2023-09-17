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

	nSamples = nChunkSize / (wavHeader.nChannels * (wavHeader.wBitsPerSample >> 3));
	nChannels = wavHeader.nChannels;

	fSample = new float[nSamples * nChannels];
	float* pSample = fSample;

	for (long i = 0; i < nSamples; ++i)
	{
		for (int c = 0; c < nChannels; ++c)
		{
			short s = 0;
			std::fread(&s, sizeof(short), 1, f);
			*pSample = (float)s / (float)(MAXSHORT);
			++pSample;
		}
	}

	std::fclose(f);
	bSampleValid = true;

	WAVEFORMATEX wavHeader;
	float* fSample = nullptr;
	long nSamples = 0;
	int nChannels = 0;
	bool bSampleValid = false; 

}

unsigned int AudioEngine::LoadAudioSample(std::wstring sWavFile)
{
	if (1/*!_bEnableSound*/)
		return -1;
	AudioSample a(sWavFile);
	if (a.bSampleValid)
	{
		vecAudioSamples.push_back(a);
		return vecAudioSamples.size();
	}
	else
		return -1 ;
}

void AudioEngine::PlaySample(int id, bool bLoop)
{
	sCurrentlyPlayingSample a;
	a.nAudioSampleID = id;
	a.nSamplePosition = 0;
	a.bFinished = false;
	a.bLoop = bLoop;
	listActiveSamples.push_back(a);
}


void AudioEngine::StopSample(int id)
{

}