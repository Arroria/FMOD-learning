#pragma once
#include "fmod.hpp"
#include <string_view>

class SoundDevice;
class SoundChannel
{
public:
	SoundChannel();
	SoundChannel(SoundSample& soundSample);
	SoundChannel(SoundChannel&& soundChannel);
	~SoundChannel();

	SoundChannel& operator=(SoundChannel&& soundChannel);


	void play();
	void pause();
	void stop();
	void detach();

private:
	FMOD::Channel* m_channel;
};

class SoundSample
{
public:
	SoundSample();
	SoundSample(SoundDevice& soundDevice, std::string_view filePath);
	SoundSample(SoundSample&& soundSample);
	~SoundSample();

	SoundSample& operator=(SoundSample&& soundSample);


	SoundChannel play();
	void release();

private:
	SoundDevice* m_myDevice;
	FMOD::Sound* m_sound;
};

class SoundDevice
{
public:
	SoundDevice();
	~SoundDevice();

	void Initialize();
	void Release();

	SoundSample CreateSoundSample(std::string_view filePath);

	FMOD::System* _get_fmod_system();

private:
	FMOD::System* m_system;
};

