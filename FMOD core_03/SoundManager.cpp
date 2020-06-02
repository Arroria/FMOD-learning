#include "SoundManager.h"

bool _fmod_failed(FMOD_RESULT result) { return FMOD_RESULT::FMOD_OK != result; }
#define _fmod_failed_return_false(result) { if (_fmod_failed(result)) return false; }



FMOD::System* __fmod_raw_transform(SoundManager& soundManager)	{ return soundManager.m_system; }
FMOD::Sound* __fmod_raw_transform(Sound& soundData)				{ return soundData.m_sound; }
FMOD::Channel* __fmod_raw_transform(SoundChannel& soundChannel)	{ return soundChannel.m_channel; }



FMOD::Sound* __raw_sound_load(FMOD::System* system, std::string_view soundFilePath)
{
	FMOD::Sound* _output_sound = nullptr;
	if (_fmod_failed(system->createSound(soundFilePath.data(), FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &_output_sound)))
		return nullptr;
	return _output_sound;
}
FMOD::Channel* __raw_sound_play(FMOD::System* system, FMOD::Sound* sound)
{
	FMOD::Channel* _output_channel= nullptr;
	if (_fmod_failed(system->playSound(sound, nullptr, true, std::addressof(_output_channel))))
		return nullptr;
	return _output_channel;
}





//////////////////
// SoundManager //
//////////////////

SoundManager::SoundManager()
	: m_system(nullptr)
{
}

SoundManager::~SoundManager()
{
	Release();
}



bool SoundManager::Initialize(int availableChannelCount)
{
	// channel count : 0 ~ 4093
	if (is_system_available()) return false;
	if (_fmod_failed(FMOD::System_Create(&m_system))) return false;
	if (_fmod_failed(m_system->init(availableChannelCount, FMOD_INIT_NORMAL, nullptr)))
	{
		m_system->release();
		m_system = nullptr;
	}
	return true;
}

void SoundManager::Update()
{
	if (is_system_available())
		if (_fmod_failed(m_system->update()))
			int error = 1;
}

void SoundManager::Release()
{
	if (!is_system_available()) return;
	m_system->close();
	m_system->release();
	m_system = nullptr;
}



Sound SoundManager::LoadSound(std::string_view soundFilePath)	{ return Sound(this, soundFilePath); }
//SoundChannel SoundManager::PlaySound(Sound& sound)				{ return SoundChannel(sound); }

bool SoundManager::is_system_available() const { return m_system; }





///////////
// Sound //
///////////

Sound::Sound()
	: m_parent(nullptr)
	, m_sound(nullptr)
	, m_refCount(nullptr)
{
}
Sound::Sound(SoundManager* baseSoundManager, std::string_view soundFilePath)
	: Sound()
{
	if (!baseSoundManager) return;
	FMOD::Sound* sound = __raw_sound_load(__fmod_raw_transform(*baseSoundManager), soundFilePath);
	if (!sound) return;

	_apply_sound(baseSoundManager, sound, nullptr);
}
Sound::Sound(Sound& refTarget)
	: Sound()
{
	if (!refTarget.is_available()) return;
	_apply_sound(refTarget);
}
Sound::Sound(Sound&& rvalue)
	: Sound()
{
	if (!rvalue.is_available()) return;
	_move_sound(std::move(rvalue));
}
Sound::~Sound()
{
	Release();
}



Sound& Sound::operator=(Sound& refTarget)
{
	if (m_sound != refTarget.m_sound)
	{
		Release();
		if (refTarget.is_available())
			_apply_sound(refTarget);
	}
	return *this;
}

Sound& Sound::operator=(Sound&& rvalue)
{
	if (m_sound != rvalue.m_sound)
	{
		Release();
		if (rvalue.is_available())
			_move_sound(std::move(rvalue));
	}
	else
		rvalue.Release();
	return *this;
}



bool Sound::is_available() const	{ return m_sound; }
SoundManager* Sound::BasedManager() { return m_parent; }
size_t Sound::RefCount() const { return m_refCount ? *m_refCount : 0; }

void Sound::_refCound_up()
{
	if (RefCount())
		++(*m_refCount);
	else
		m_refCount = new int(1);
}
void Sound::_refCound_down()
{
	if (RefCount())
	{
		if (!--(*m_refCount))
		{
			delete m_refCount;
			m_refCount = nullptr;
		}
	}
	else
		int error = -1;
}


void Sound::_apply_sound(SoundManager* baseSoundManager, FMOD::Sound* _raw_sound, int* refCounter)
{
	m_parent = baseSoundManager;
	m_sound = _raw_sound;
	m_refCount = refCounter;
	_refCound_up();
}
void Sound::_apply_sound(Sound& copyTarget) { _apply_sound(copyTarget.m_parent, copyTarget.m_sound, copyTarget.m_refCount); }
void Sound::_move_sound(Sound&& rvalue)
{
	m_parent = rvalue.m_parent;
	m_sound = rvalue.m_sound;
	m_refCount = rvalue.m_refCount;
	rvalue.m_parent = nullptr;
	rvalue.m_sound = nullptr;
	rvalue.m_refCount = nullptr;
}



SoundChannel Sound::PrepareChannel()	{ return SoundChannel(*this); }
void Sound::Release()
{
	if (!is_available())
		return;

	_refCound_down();
	if (!RefCount())
		m_sound->release(); 
	m_sound = nullptr;
	m_refCount = nullptr;
	m_parent = nullptr;
}


std::string Sound::name() const
{
	constexpr size_t _name_length = 260;
	char name[_name_length + 1] = "";
	m_sound->getName(name, _name_length);
	return name;
}
size_t Sound::length() const
{
	unsigned int length = 0;
	m_sound->getLength(&length, FMOD_TIMEUNIT_MS);
	return length;
}





//////////////////
// SoundChannel //
//////////////////

SoundChannel::SoundChannel()
	: m_parent()
	, m_channel(nullptr)
{
}
SoundChannel::SoundChannel(Sound& basedSound)
{
	if (!basedSound.is_available()) return;
	FMOD::Channel* channel = __raw_sound_play(__fmod_raw_transform(*basedSound.BasedManager()), __fmod_raw_transform(basedSound));
	if (!channel) return;

	m_parent = basedSound;
	m_channel = channel;
}
SoundChannel::SoundChannel(SoundChannel&& rvalue)
{
	_move_channel(std::move(rvalue));
}
SoundChannel::~SoundChannel()
{
	Release();
}

SoundChannel& SoundChannel::operator=(SoundChannel&& rvalue)
{
	_move_channel(std::move(rvalue));
	return *this;
}

void SoundChannel::_move_channel(SoundChannel&& rvalue)
{
	m_parent = std::move(rvalue.m_parent);
	m_channel = rvalue.m_channel;
	rvalue.m_channel = nullptr;
}



bool SoundChannel::is_available() const
{
	bool available(false);
	if (m_channel)
		m_channel->isPlaying(&available);
	return available;
}
Sound& SoundChannel::BasedSound()				{ return m_parent; }
const Sound& SoundChannel::BasedSound() const	{ return m_parent; }
bool SoundChannel::is_parent_available() const	{ return m_parent.is_available(); }
bool SoundChannel::is_auto_recycled() const		{ return !is_available() && is_parent_available(); }


void SoundChannel::Reallocation()
{
	if (is_parent_available())
	{
		m_channel->stop();
		m_channel = nullptr;
		_move_channel(std::move(m_parent.PrepareChannel()));
	}
}
void SoundChannel::Release()
{
	if (!is_available()) // == (is_available() || is_auto_recycled())
		return;

	m_channel->stop();
	m_channel = nullptr;
	m_parent.Release();
}

void SoundChannel::Play()
{
	if (!is_available())
	{
		if (is_auto_recycled())
			Reallocation();
		else
			return;
	}
	Pause(false);
}
void SoundChannel::Pause(bool pause)
{
	if (!is_available()) return;
	m_channel->setPaused(pause);
}
void SoundChannel::Stop()
{
	if (!is_available())
	{
		if (is_auto_recycled())
			Reallocation(); // default paused
		else
			return;
	}
	else
	{
		Pause(true);
		SetTimePosition(0);
	}
}
void SoundChannel::SetTimePosition(size_t position)
{
	if (!is_available()) return;
	m_channel->setPosition(position, FMOD_TIMEUNIT_MS);
}

void SoundChannel::Mute(bool mute)
{
	if (!is_available()) return;
	m_channel->setMute(mute);
}
void SoundChannel::SetVolum(float volum)
{
	if (!is_available()) return;
	m_channel->setVolume(volum);
}


void __throw_unavailable() { throw std::runtime_error("unavailable sound channel"); }
bool SoundChannel::IsPlaying() { return !IsEnded() && !IsPaused(); }
bool SoundChannel::IsEnded()
{
	return is_auto_recycled();
	// OLD CODE
	//if (!is_available()) __throw_unavailable();
	//size_t length = m_parent.length();
	//size_t position = GetTimePosition();
	//return length <= position;
}
bool SoundChannel::IsPaused()
{
	if (!is_available())
	{
		if (is_auto_recycled())
			return true;
		else
			__throw_unavailable();
	}
	bool paused = NULL;
	m_channel->getPaused(&paused);
	return paused;
}
size_t SoundChannel::GetTimePosition()
{
	if (!is_available())
		if (!is_auto_recycled())
			__throw_unavailable();
	
	// 버그 발생 가능성 있음
	unsigned int position = NULL;
	m_channel->getPosition(&position, FMOD_TIMEUNIT_MS);
	return position;
}

bool SoundChannel::IsMuted()
{
	if (!is_available())
		if (!is_auto_recycled())
			__throw_unavailable();

	// 버그 발생 가능성 있음
	bool muted = NULL;
	m_channel->getMute(&muted);
	return muted;
}
float SoundChannel::GetVolum()
{
	if (!is_available())
		if (!is_auto_recycled())
			__throw_unavailable();

	// 버그 발생 가능성 있음
	float volum = NULL;
	m_channel->getVolume(&volum);
	return volum;
}
