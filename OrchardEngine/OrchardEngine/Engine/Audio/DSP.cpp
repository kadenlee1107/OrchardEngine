#include "DSP.hpp"
#include <cstring>
#include <algorithm>

namespace Orchard::Audio::DSP {

void Resampler::Resample(const float* input, size_t inputSamples, uint32_t inputRate,
                        float* output, size_t outputSamples, uint32_t outputRate) {
    double ratio = static_cast<double>(inputRate) / static_cast<double>(outputRate);
    
    for (size_t i = 0; i < outputSamples; ++i) {
        double srcIndex = i * ratio;
        size_t index0 = static_cast<size_t>(srcIndex);
        size_t index1 = std::min(index0 + 1, inputSamples - 1);
        
        double frac = srcIndex - index0;
        
        output[i] = input[index0] * (1.0 - frac) + input[index1] * frac;
    }
}

Reverb::Reverb() {
    const size_t combSizes[] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
    const size_t allpassSizes[] = {556, 441, 341, 225};
    
    for (size_t size : combSizes) {
        CombFilter filter;
        filter.buffer.resize(size, 0.0f);
        filter.index = 0;
        filter.feedback = 0.5f;
        m_CombFilters.push_back(filter);
    }
    
    for (size_t size : allpassSizes) {
        AllpassFilter filter;
        filter.buffer.resize(size, 0.0f);
        filter.index = 0;
        filter.feedback = 0.5f;
        m_AllpassFilters.push_back(filter);
    }
}

void Reverb::Process(float* buffer, size_t sampleCount, uint32_t channels) {
    for (size_t i = 0; i < sampleCount; ++i) {
        float input = buffer[i];
        float output = 0.0f;
        
        for (auto& comb : m_CombFilters) {
            float delayed = comb.buffer[comb.index];
            float filtered = delayed * (1.0f - comb.damp) + comb.filterStore * comb.damp;
            comb.filterStore = filtered;
            
            comb.buffer[comb.index] = input + filtered * comb.feedback;
            
            comb.index = (comb.index + 1) % comb.buffer.size();
            
            output += filtered;
        }
        
        output /= m_CombFilters.size();
        
        for (auto& allpass : m_AllpassFilters) {
            float delayed = allpass.buffer[allpass.index];
            float temp = output - delayed * allpass.feedback;
            
            allpass.buffer[allpass.index] = temp;
            output = delayed + temp * allpass.feedback;
            
            allpass.index = (allpass.index + 1) % allpass.buffer.size();
        }
        
        buffer[i] = input * m_DryLevel + output * m_WetLevel;
    }
}

void EQ::SetFilter(FilterType type, float frequency, float q, float gain, uint32_t sampleRate) {
    m_Type = type;
    
    float w0 = 2.0f * 3.14159265f * frequency / sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * q);
    float A = std::pow(10.0f, gain / 40.0f);
    
    switch (type) {
        case FilterType::LowPass:
            m_B0 = (1.0f - cosw0) / 2.0f;
            m_B1 = 1.0f - cosw0;
            m_B2 = (1.0f - cosw0) / 2.0f;
            m_A0 = 1.0f + alpha;
            m_A1 = -2.0f * cosw0;
            m_A2 = 1.0f - alpha;
            break;
            
        case FilterType::HighPass:
            m_B0 = (1.0f + cosw0) / 2.0f;
            m_B1 = -(1.0f + cosw0);
            m_B2 = (1.0f + cosw0) / 2.0f;
            m_A0 = 1.0f + alpha;
            m_A1 = -2.0f * cosw0;
            m_A2 = 1.0f - alpha;
            break;
            
        case FilterType::Peak:
            m_B0 = 1.0f + alpha * A;
            m_B1 = -2.0f * cosw0;
            m_B2 = 1.0f - alpha * A;
            m_A0 = 1.0f + alpha / A;
            m_A1 = -2.0f * cosw0;
            m_A2 = 1.0f - alpha / A;
            break;
            
        default:
            break;
    }
    
    m_B0 /= m_A0;
    m_B1 /= m_A0;
    m_B2 /= m_A0;
    m_A1 /= m_A0;
    m_A2 /= m_A0;
}

void EQ::Process(float* buffer, size_t sampleCount, uint32_t channels) {
    for (size_t i = 0; i < sampleCount; ++i) {
        float input = buffer[i];
        
        float output = m_B0 * input + m_B1 * m_X1 + m_B2 * m_X2
                     - m_A1 * m_Y1 - m_A2 * m_Y2;
        
        m_X2 = m_X1;
        m_X1 = input;
        m_Y2 = m_Y1;
        m_Y1 = output;
        
        buffer[i] = output;
    }
}

void Compressor::Process(float* buffer, size_t sampleCount, uint32_t sampleRate) {
    float attackCoeff = std::exp(-1.0f / (m_Attack * sampleRate));
    float releaseCoeff = std::exp(-1.0f / (m_Release * sampleRate));
    
    for (size_t i = 0; i < sampleCount; ++i) {
        float input = std::abs(buffer[i]);
        
        float targetEnvelope = input;
        if (m_Envelope < targetEnvelope) {
            m_Envelope = attackCoeff * m_Envelope + (1.0f - attackCoeff) * targetEnvelope;
        } else {
            m_Envelope = releaseCoeff * m_Envelope + (1.0f - releaseCoeff) * targetEnvelope;
        }
        
        float envelopeDb = LinearToDecibel(m_Envelope);
        float gainReduction = 0.0f;
        
        if (envelopeDb > m_Threshold) {
            float excess = envelopeDb - m_Threshold;
            gainReduction = excess * (1.0f - 1.0f / m_Ratio);
        }
        
        float gain = DecibelToLinear(-gainReduction);
        
        buffer[i] *= gain;
    }
}

}
