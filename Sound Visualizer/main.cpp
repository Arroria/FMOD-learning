#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <chrono>


#include <string_view>
constexpr std::string_view _path_test[] = 
{ "C:/.Arroria/FMOD Studio API Windows/api/core/examples/media/wave.mp3"
,"C:/.Arroria/Visual Studio Project/_2019/Rhythm Game/rhy_03/697873717765.mp3"
,"bt3tm.mp3"
,"i.mp3"
,"over.mp3"
,"anni.mp3"
,"ear careful test.mp3"
,"nightmare.mp3"
,"dest.mp3"
,"aleph.mp3"
};


#include "ConsoleDoubleBuffer.h"
#include "SoundVisualizer.h"

int main()
{
	Sleep(1000);
	ConsoleDoubleBuffer cdb(300);

	SoundDevice sd;
	sd.Initialize();

	SoundSample sound(sd, _path_test[3]);
	{
		SoundChannel sc(sound);
		SoundVisualizer sv;
		sv.Initialize(sd, &sc, 7, 100);


		auto start = std::chrono::steady_clock::now();

		SoundVisualizer_CalculateType svcType = SoundVisualizer_CalculateType::Average;
		while (sc.is_available())
		{
			if (GetAsyncKeyState('1'))	svcType = SoundVisualizer_CalculateType::Average;
			if (GetAsyncKeyState('2'))	svcType = SoundVisualizer_CalculateType::AverageOffsetting;
			if (GetAsyncKeyState('3'))	svcType = SoundVisualizer_CalculateType::Maximum;
			if (GetAsyncKeyState('4'))	svcType = SoundVisualizer_CalculateType::MaximumOffsetting;

			if (GetAsyncKeyState('Q'))	sv.SetOffsetSetting(20);	if (GetAsyncKeyState('A'))	sv.SetOffsetSetting(120);
			if (GetAsyncKeyState('W'))	sv.SetOffsetSetting(40);	if (GetAsyncKeyState('S'))	sv.SetOffsetSetting(140);
			if (GetAsyncKeyState('E'))	sv.SetOffsetSetting(60);	if (GetAsyncKeyState('D'))	sv.SetOffsetSetting(160);
			if (GetAsyncKeyState('R'))	sv.SetOffsetSetting(80);	if (GetAsyncKeyState('F'))	sv.SetOffsetSetting(180);
			if (GetAsyncKeyState('T'))	sv.SetOffsetSetting(100);	if (GetAsyncKeyState('G'))	sv.SetOffsetSetting(200);


			const SoundVisualizer::SpectrumDataBuffer_t& spectrumData = sv.SpectrumDataCalculate(svcType);
			
			


			constexpr size_t max_y = 63;
			constexpr size_t render_y = 50;
			cdb.Clear();
			cdb.Begin();

			for (size_t i = 0; i < spectrumData.size(); i++)
			{
				int length = spectrumData[i] * render_y
				*((svcType != SoundVisualizer_CalculateType::AverageOffsetting && svcType != SoundVisualizer_CalculateType::MaximumOffsetting)
					? 5 :1);

				for (size_t y = 0; y < length; y++)
				{
					if (y > max_y)
						break;
					cdb.CursorTo(i * 2, max_y - y);
					std::cout << "бс";
				}
			}

			auto end = std::chrono::steady_clock::now();
			cdb.CursorTo(0, 0);
			std::cout << (std::chrono::nanoseconds(std::chrono::seconds(1)) / (end-start));
			start = end;


			cdb.End();
			cdb.Flipping();
		}
		sv.Release();
	}
	sound.release();
	sd.Release();
	return 0;
}
