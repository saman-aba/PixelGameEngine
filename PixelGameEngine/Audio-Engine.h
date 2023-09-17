#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include <list>
#include <thread>
#include <condition_variable>
#include <atomic>

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

	

	WAVEFORMATEX wavHeader{};
	float* fSample = nullptr;
	long nSamples = 0;
	int nChannels = 0;
	bool bSampleValid = false;

};

class AudioEngine
{
public:
	AudioEngine();
	unsigned int LoadAudioSample(std::wstring sWavFile);
	void PlaySample(int id, bool Loop = false);
	void StopSample(int id);
	bool CreateAudio(unsigned int nSampleRate = 44100, unsigned int nChannels = 1, unsigned int nBlocks = 8, unsigned int nBlockSamples = 512);
	bool DestroyAudio();
	void waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2);
	static void CALLBACK waveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	void AudioThread();

	virtual float onUserSoundSample(int nChannel, float fGlobalTime, float fTimeStep);
	virtual float onUserSoundFilter(int nChannel, float fGlobalTime, float fSample);
	float GetMixerOutput(int nChannel, float fGlobalTime, float fTimeStep);
private:
	std::vector<AudioSample> vecAudioSamples;
	std::list<sCurrentlyPlayingSample> listActiveSamples;


	unsigned int m_nSampleRate;
	unsigned int m_nChannels;
	unsigned int m_nBlockCount;
	unsigned int m_nBlockSamples;
	unsigned int m_nBlockCurrent;

	short* m_pBlockMemory = nullptr;
	WAVEHDR* m_pWaveHeaders = nullptr;
	HWAVEOUT m_hwDevice = nullptr;

	std::thread m_AudioThread;
	std::atomic<bool> m_bAudioThreadActive = false;
	std::atomic<unsigned int> m_nBlockFree = 0;
	std::condition_variable m_cvBlockNotZero;
	std::mutex m_muxBlockNotZero;
	std::atomic<float> m_fGlobalTime = 0.0f;
};