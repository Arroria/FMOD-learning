#include "SoundVisualizer.h"
#include <algorithm>
#include <cmath>

// 참고 자료
// https://www.parallelcube.com/2018/03/10/frequency-spectrum-using-fmod-and-ue4/

SoundVisualizer::SoundVisualizer()
	: m_dsp(nullptr)
	, m_channel(nullptr)


	, m_spectrumLength(NULL)
	, m_spectrumSampleAccuracy(NULL)
	, m_spectrumUnits()
	, m_spectrumOffsetMagnification(0)

	, m_spectrumDataBuffer()
{
}

SoundVisualizer::~SoundVisualizer()
{
}



bool SoundVisualizer::Initialize(SoundDevice& device, SoundChannel* channel, size_t spectrumSampleAccuracyLevel, size_t spectrumLength)
{
	m_channel = channel;
	m_spectrumLength = spectrumLength;
	m_spectrumSampleAccuracy = 2 << (spectrumSampleAccuracyLevel + 6);
	if (m_spectrumSampleAccuracy < (2 << 6) ||
		m_spectrumSampleAccuracy > (2 << 13))
		return false;

	device._get_primitive_system()->createDSPByType(FMOD_DSP_TYPE_FFT, &m_dsp);
	m_dsp->setParameterInt(FMOD_DSP_FFT_WINDOWTYPE, FMOD_DSP_FFT_WINDOW_RECT);
	m_dsp->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, m_spectrumSampleAccuracy);
	m_channel->_addDSP(m_dsp);



	const double unitMagnification = std::pow(m_spectrumSampleAccuracy, 1.0 / (double)m_spectrumLength);
	double currentMagnification = 1;
	m_spectrumUnits.resize(m_spectrumLength);
	for (int index = 0; index < m_spectrumLength; ++index)
	{
		double prevMagnification = currentMagnification;
		currentMagnification *= unitMagnification;
		m_spectrumUnits[index] = std::move(std::make_pair((int)prevMagnification, (int)currentMagnification));
	}

	SetOffsetSetting(100);
	return true;
}

void SoundVisualizer::SetOffsetSetting(double spectrumOffsetCoefficient)
{
	m_spectrumOffsetMagnification = std::pow(spectrumOffsetCoefficient, 1.0 / (double)m_spectrumLength);
}

void SoundVisualizer::Release()
{
	if (m_channel)
	{
		m_channel->_removeDSP(m_dsp);
		m_channel = nullptr;
	}
	if (m_dsp)
	{
		m_dsp->release();
		m_dsp = nullptr;
	}
}



const SoundVisualizer::SpectrumDataBuffer_t& SoundVisualizer::SpectrumDataCalculate(SoundVisualizer_CalculateType calculateType) const
{
	const bool isAverage = calculateType == SoundVisualizer_CalculateType::Average || calculateType == SoundVisualizer_CalculateType::AverageOffsetting;
	const bool isOffsetting = calculateType == SoundVisualizer_CalculateType::AverageOffsetting || calculateType == SoundVisualizer_CalculateType::MaximumOffsetting;
	m_spectrumDataBuffer.assign(m_spectrumLength, 0);


	FMOD_DSP_PARAMETER_FFT* fftData = nullptr;
	m_dsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void**)& fftData, 0, 0, 0);
	if (fftData && fftData->length / 2 > 0)
	{
		for (int index = 0; index < m_spectrumLength; ++index)
		{
			const auto& unit = m_spectrumUnits[index];
			auto& dataBuf = m_spectrumDataBuffer[index];

			const int sampleBegin = unit.first;
			const int sampleEnd = unit.second;
			const int sampleCount = sampleEnd - sampleBegin + 1;

			for (int channel = 0; channel < fftData->numchannels; ++channel)
			{
				float* channelValue = fftData->spectrum[channel];
				for (int frec = sampleBegin; frec <= sampleEnd; ++frec)
				{
					float value = channelValue[frec];
					if (isAverage)
						dataBuf += value;
					else
					{
						if (dataBuf < value)
							dataBuf = value;
					}
				}
			}
			if (isAverage)
				dataBuf /= (float)(sampleCount * fftData->numchannels);
		}

		if (isOffsetting)
		{
			double currentMagnification = 1;
			for (int index = 0; index < m_spectrumLength; ++index)
			{
				m_spectrumDataBuffer[index] *= currentMagnification;
				currentMagnification *= m_spectrumOffsetMagnification;
			}
		}
	}
	return m_spectrumDataBuffer;
}
