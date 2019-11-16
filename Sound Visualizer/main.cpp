#include <fmod.hpp>
#include <Windows.h>


#include <string_view>
constexpr std::string_view _path_test = "C:/.Arroria/FMOD Studio API Windows/api/core/examples/media/wave.mp3";

int main()
{
	FMOD::System* sys;
	FMOD::System_Create(&sys);
	sys->init(256, 0, nullptr);

	FMOD::Sound* sound;
	auto error =  sys->createSound(_path_test.data(), FMOD_LOOP_OFF, nullptr, &sound);
	
	{
		FMOD::Channel* c;
		sys->playSound(sound, nullptr, false, &c);

		Sleep(10000);
	}

	sound->release();
	sys->release();
	return 0;
}
