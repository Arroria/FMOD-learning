#pragma once
#include "../FMOD core_02/SoundDevice.h"
#include <vector>

enum class SoundVisualizer_CalculateType
{
	Average,
	AverageOffsetting,
	Maximum,
	MaximumOffsetting,
};

class SoundVisualizer
{
public:
	using SpectrumDataBuffer_t = std::vector<float>;


	SoundVisualizer();
	~SoundVisualizer();

	bool Initialize(SoundDevice& device, SoundChannel* channel, size_t spectrumSampleAccuracyLevel, size_t spectrumLength);
	void SetOffsetSetting(double spectrumOffsetCoefficient);
	void Release();

	const SpectrumDataBuffer_t& SpectrumDataCalculate(SoundVisualizer_CalculateType calculateType) const;


	size_t SpectrumSampleAccuracy() const { return m_spectrumSampleAccuracy; }


private:
	FMOD::DSP* m_dsp;
	SoundChannel* m_channel;


	size_t m_spectrumLength;
	size_t m_spectrumSampleAccuracy;
	std::vector<std::pair<int, int>> m_spectrumUnits;
	double m_spectrumOffsetMagnification;

	mutable SpectrumDataBuffer_t m_spectrumDataBuffer;
};

