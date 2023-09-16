#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include <list>

struct sCurrentlyPlayingSample
{
	int nAudioSampleID;
	long nSamplePosition;
	bool bFinished = false;
	bool bLoop = false;
};

class AudioSample
{
public:
	AudioSample();
	AudioSample(std::wstring sWavFile);

	WAVEFORMATEX wavHeader;
	float* fSample = nullptr;
	long nSamples = 0;
	int nChannels = 0;
	bool bSampleValid = false;
};

class AudioEngine
{
public:
	AudioEngine();


private:
	std::vector<AudioSample> vecAudioSamples;
	std::list<sCurrentlyPlayingSample> listActiveSamples;
};