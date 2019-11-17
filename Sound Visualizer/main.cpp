#include <fmod.hpp>
#define NOMINMAX
#include <Windows.h>
#include <iostream>
// 참고 자료
// https://www.parallelcube.com/2018/03/10/frequency-spectrum-using-fmod-and-ue4/


#include <string_view>
constexpr std::string_view _path_test[] = 
{ "C:/.Arroria/FMOD Studio API Windows/api/core/examples/media/wave.mp3"
,"C:/.Arroria/Visual Studio Project/_2019/Rhythm Game/rhy_03/697873717765.mp3"
,"C:/.Arroria/Visual Studio Project/_2019/FMOD learning/Sound Visualizer/bt3tm.mp3"
,"C:/.Arroria/Visual Studio Project/_2019/FMOD learning/Sound Visualizer/i.mp3"
,"C:/.Arroria/Visual Studio Project/_2019/FMOD learning/Sound Visualizer/over.mp3"
,"C:/.Arroria/Visual Studio Project/_2019/FMOD learning/Sound Visualizer/anni.mp3"
,"C:/.Arroria/Visual Studio Project/_2019/FMOD learning/Sound Visualizer/ear careful test.mp3"
,"C:/.Arroria/Visual Studio Project/_2019/FMOD learning/Sound Visualizer/nightmare.mp3"
};


#include "ConsoleDoubleBuffer.h"

#include <vector>
#include <cmath>
#include <algorithm>

int main()
{
	ConsoleDoubleBuffer cdb(300);
	constexpr int sound_index = 4;

	FMOD::System* sys;
	FMOD::System_Create(&sys);
	sys->init(256, 0, nullptr);

	FMOD::Sound* sound;
	auto error = sys->createSound(_path_test[sound_index].data(), FMOD_LOOP_OFF, nullptr, &sound);

	{
		/*
		윈도 사이즈는 128~16384(2^7 ~ 2^15)범위의 2의 제곱수 값만 사용 가능
		나중에 반환되는 FFT spectrum의 길이 값과 동일

		*/



		constexpr bool isLinear = false;
		constexpr bool isCustom = true;
		constexpr int winS =
			2 << 13
			//128
			//1024
			//16384
			;
		FMOD::Channel* c;
		sys->playSound(sound, nullptr, false, &c);

		FMOD::DSP* dsp;
		sys->createDSPByType(FMOD_DSP_TYPE_FFT, &dsp);
		dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_RECT);
		dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, winS);
		error = c->addDSP(0, dsp);


		const double __fx = std::pow(100, 1.0 / (double)100);

		std::vector<std::pair<int, int>> myBar;
		auto initBar = [&](int size)->int
		{
			const double fx = std::pow(winS, 1.0 / (double)size);
			double pos = 1;
			for (int i = 0; i < size; ++i)
			{
				double prevPos = pos;
				pos *= fx;
				myBar.push_back(std::make_pair((int)prevPos, (int)pos));
			}
			return myBar.size();
		};
		initBar(100);
		while (true)
		{
			std::vector <float> spectrumData(100);
			FMOD_DSP_PARAMETER_FFT* fftData = nullptr;
			dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void**)& fftData, 0, 0, 0);
			if (fftData && fftData->length / 2 > 0)
			{
				for (int index = 0; index < myBar.size(); ++index)
				{
					int begin = myBar[index].first;
					int end = myBar[index].second;
					int length = end - begin + 1;
					for (int frec = begin; frec <= end; ++frec)
					{
						for (int channel = 0; channel < fftData->numchannels; ++channel)
							spectrumData[index] = std::max(spectrumData[index], fftData->spectrum[channel][frec]);
						//spectrumData[index] += fftData->spectrum[channel][frec];
					}
					//spectrumData[index] /= (float)(length * fftData->numchannels);
				}
			}



			cdb.Clear();
			cdb.Begin();

			for (size_t i = 0; i < 100; i++)
			{
				int length = spectrumData[i] * 50 * 2.5;// *std::pow(__fx, i);
				for (size_t y = 0; y < length; y++)
				{
					if (y > 50)
						break;
					cdb.CursorTo(i * 2, 50 - y);
					std::cout << "■";
				}
			}

			cdb.End();
			cdb.Flipping();
			Sleep(20);
		}
	}

	sound->release();
	sys->release();
	return 0;
}
