#pragma once
#include "fmod.hpp"

#include <string_view>
#include <stdexcept>

class Sound;
class SoundChannel;
class SoundManager;

class Sound
{
public:
	Sound();
	Sound(SoundManager* baseSoundManager, std::string_view soundFilePath);
	Sound(Sound& refTarget);
	Sound(Sound&& rvalue);
	~Sound();
	Sound& operator=(Sound& refTarget);
	Sound& operator=(Sound&& rvalue);


	bool is_available() const;
	SoundManager* BasedManager();
	size_t RefCount() const;
	friend FMOD::Sound* __fmod_raw_transform(Sound&);


	SoundChannel PrepareChannel();
	void Release();

	std::string name() const;
	size_t length() const;

private:
	// Management
	void _refCound_up();
	void _refCound_down();

	void _apply_sound(SoundManager* baseSoundManager, FMOD::Sound* _raw_sound, int* refCounter);
	void _apply_sound(Sound& copyTarget);
	void _move_sound(Sound&& rvalue);

private:
	SoundManager* m_parent;
	FMOD::Sound* m_sound;
	int* m_refCount;
};

class SoundChannel
{
public:
	SoundChannel();
	SoundChannel(Sound& basedSound);
	SoundChannel(SoundChannel&& rvalue);
	~SoundChannel();
	SoundChannel& operator=(SoundChannel&& rvalue);

	bool is_available() const;
	Sound& BasedSound();
	const Sound& BasedSound() const;
	friend FMOD::Channel* __fmod_raw_transform(SoundChannel&);
	bool is_parent_available() const;
	bool is_auto_recycled() const;
	
	void Reallocation();
	void Release();

	// option set
	void Play();
	void Pause(bool pause);
	void Stop();
	void SetTimePosition(size_t position);
	
	void Mute(bool mute);
	void SetVolum(float volum);

	// option get
	bool IsPlaying();
	bool IsEnded();
	bool IsPaused() throw(std::runtime_error);
	size_t GetTimePosition() throw(std::runtime_error);
	
	bool IsMuted() throw(std::runtime_error);
	float GetVolum() throw(std::runtime_error);


private:
	void _move_channel(SoundChannel&& rvalue);

private:
	Sound m_parent;
	FMOD::Channel* m_channel;
};



class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	bool Initialize(int availableChannelCount);
	void Update();
	void Release();

	Sound LoadSound(std::string_view soundFilePath);
	SoundChannel PlaySound(Sound& sound);

	bool is_system_available() const;
	friend FMOD::System* __fmod_raw_transform(SoundManager&);

private:
	FMOD::System* m_system;
};
