#pragma once

#include <vector>
#include <cmath>

namespace Orchard::Audio::DSP {

class Resampler {
public:
    static void Resample(const float* input, size_t inputSamples, uint32_t inputRate,
                        float* output, size_t outputSamples, uint32_t outputRate);
};

class Reverb {
public:
    Reverb();
    
    void SetRoomSize(float size) { m_RoomSize = size; }
    void SetDamping(float damping) { m_Damping = damping; }
    void SetWetLevel(float wet) { m_WetLevel = wet; }
    void SetDryLevel(float dry) { m_DryLevel = dry; }
    
    void Process(float* buffer, size_t sampleCount, uint32_t channels);
    
private:
    struct CombFilter {
        std::vector<float> buffer;
        size_t index = 0;
        float feedback = 0.5f;
        float filterStore = 0.0f;
        float damp = 0.4f;
    };
    
    struct AllpassFilter {
        std::vector<float> buffer;
        size_t index = 0;
        float feedback = 0.5f;
    };
    
    std::vector<CombFilter> m_CombFilters;
    std::vector<AllpassFilter> m_AllpassFilters;
    
    float m_RoomSize = 0.5f;
    float m_Damping = 0.5f;
    float m_WetLevel = 0.3f;
    float m_DryLevel = 0.7f;
};

class EQ {
public:
    enum class FilterType {
        LowPass,
        HighPass,
        BandPass,
        Notch,
        Peak,
        LowShelf,
        HighShelf
    };
    
    void SetFilter(FilterType type, float frequency, float q, float gain, uint32_t sampleRate);
    void Process(float* buffer, size_t sampleCount, uint32_t channels);
    
private:
    FilterType m_Type = FilterType::Peak;
    float m_A0 = 1.0f, m_A1 = 0.0f, m_A2 = 0.0f;
    float m_B0 = 1.0f, m_B1 = 0.0f, m_B2 = 0.0f;
    
    float m_X1 = 0.0f, m_X2 = 0.0f;
    float m_Y1 = 0.0f, m_Y2 = 0.0f;
};

class Compressor {
public:
    void SetThreshold(float threshold) { m_Threshold = threshold; }
    void SetRatio(float ratio) { m_Ratio = ratio; }
    void SetAttack(float attack) { m_Attack = attack; }
    void SetRelease(float release) { m_Release = release; }
    
    void Process(float* buffer, size_t sampleCount, uint32_t sampleRate);
    
private:
    float m_Threshold = -20.0f;
    float m_Ratio = 4.0f;
    float m_Attack = 0.005f;
    float m_Release = 0.1f;
    float m_Envelope = 0.0f;
};

inline float LinearToDecibel(float linear) {
    return 20.0f * std::log10(linear);
}

inline float DecibelToLinear(float db) {
    return std::pow(10.0f, db / 20.0f);
}

}
