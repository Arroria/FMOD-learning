#include <Windows.h>
#include <iostream>

// FMOD test sample path
constexpr std::string_view _fmod_sample_drum_path = "../../../../FMOD Studio API Windows/api/core/examples/media/drumloop.wav";
constexpr std::string_view _fmod_sample_wave_path = "../../../../FMOD Studio API Windows/api/core/examples/media/wave.mp3";

#include "SoundManager.h"
void SOUND_CLASS_TEST();

SoundManager soundManager;
Sound soundData;
SoundChannel soundChannel;
int main()
{
	soundManager.Initialize(1024);
	soundData = Sound(&soundManager, _fmod_sample_wave_path);
	soundChannel = soundData;
	SOUND_CLASS_TEST();
	soundChannel.Release();
	soundData.Release();
	soundManager.Release();
	return 0;
}
void SOUND_CLASS_TEST()
{
	while (true)
	{
		soundManager.Update();
		if (GetAsyncKeyState('1'))	soundChannel.Play();
		if (GetAsyncKeyState('2'))	soundChannel.Pause(true);
		if (GetAsyncKeyState('3'))	soundChannel.Stop();
		if (GetAsyncKeyState('4'))	soundChannel.Reallocation();
		if (GetAsyncKeyState('5'))	soundChannel.SetTimePosition(0);
		if (GetAsyncKeyState('6'))	soundChannel.SetTimePosition(1000);
		if (GetAsyncKeyState('7'))	soundChannel.SetTimePosition(10000);
		if (GetAsyncKeyState('Q'))	soundChannel.Mute(false);
		if (GetAsyncKeyState('W'))	soundChannel.Mute(true);
		if (GetAsyncKeyState('E'))	soundChannel.SetVolum(0.00);
		if (GetAsyncKeyState('R'))	soundChannel.SetVolum(0.25);
		if (GetAsyncKeyState('T'))	soundChannel.SetVolum(0.50);
		if (GetAsyncKeyState('Y'))	soundChannel.SetVolum(0.75);
		if (GetAsyncKeyState('U'))	soundChannel.SetVolum(1.00);

		system("cls");
		std::cout <<
			soundChannel.IsPlaying()		<< "\n" <<
			soundChannel.IsPaused()			<< "\n" <<
			soundChannel.IsEnded()			<< "\n" <<
			soundChannel.GetTimePosition()	<< "\n" <<
			soundChannel.IsMuted()			<< "\n" <<
			soundChannel.GetVolum()			<< "\n" <<
			std::endl;

		if (GetAsyncKeyState('0'))	soundChannel.Release();
		//if (soundChannel.IsEnded() || soundChannel.IsPaused())
		//	break;
	}
}
