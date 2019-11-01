#include <Windows.h>
#include <iostream>

#include "SoundDevice.h"

// 음원 샘플 경로
constexpr std::string_view _path_drum = "../../../../FMOD Studio API Windows/api/core/examples/media/drumloop.wav";
constexpr std::string_view _path_wave = "../../../../FMOD Studio API Windows/api/core/examples/media/wave.mp3";


int main()
{
	SoundDevice soundDevice;
	soundDevice.Initialize();

	{
		SoundSample sound1(soundDevice.CreateSoundSample(_path_drum));
		SoundSample sound2(soundDevice.CreateSoundSample(_path_wave));

		SoundChannel channel(sound1);
		Sleep(500);
		channel.stop();
		Sleep(3000);

		channel = sound2;
		Sleep(2000);
		channel.pause();
		Sleep(2000);
		channel.play();
		channel.detach();
		Sleep(5000);
	}

	soundDevice.Release();
	return 0;
}
