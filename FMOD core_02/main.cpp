#include <Windows.h>
#include <iostream>

#include "SoundDevice.h"
SoundDevice soundDevice;

// 음원 샘플 경로
constexpr std::string_view _path_drum = "../../../../FMOD Studio API Windows/api/core/examples/media/drumloop.wav";
constexpr std::string_view _path_wave = "../../../../FMOD Studio API Windows/api/core/examples/media/wave.mp3";


void testmain();
int main()
{
	soundDevice.Initialize();
	testmain();
	soundDevice.Release();
	return 0;
}

void testmain()
{
	SoundSample sound1(soundDevice.CreateSoundSample(_path_drum));
	SoundSample sound2(soundDevice.CreateSoundSample(_path_wave));

	SoundChannel channel(sound2);
	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		if (channel.is_available())
			std::cout << "time : " << channel.get_time() << "ms" << std::endl;
		else
			std::cout << "sound end" << std::endl;
	}
}
