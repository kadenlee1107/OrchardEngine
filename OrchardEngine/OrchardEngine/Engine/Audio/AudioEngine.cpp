#include "AudioEngine.hpp"
#include <iostream>
#include <cstring>

#ifdef __APPLE__
#include <AudioUnit/AudioUnit.h>
#endif

namespace Orchard::Audio {

AudioClip::AudioClip() {
}

AudioClip::~AudioClip() {
}

bool AudioClip::LoadFromFile(const std::string& path) {
    std::cout << "Loading audio clip: " << path << std::endl;
    return false;
}

bool AudioClip::LoadFromMemory(const void* data, size_t size, AudioFormat format, uint32_t sampleRate) {
    m_SampleRate = sampleRate;
    
    switch (format) {
        case AudioFormat::Mono16:
            m_Channels = 1;
            break;
        case AudioFormat::Stereo16:
            m_Channels = 2;
            break;
        default:
            m_Channels = 2;
            break;
    }
    
    size_t sampleCount = size / sizeof(int16_t);
    m_Data.resize(sampleCount);
    
    const int16_t* samples = static_cast<const int16_t*>(data);
    for (size_t i = 0; i < sampleCount; ++i) {
        m_Data[i] = samples[i] / 32768.0f;
    }
    
    m_SampleCount = static_cast<uint32_t>(sampleCount);
    
    return true;
}

AudioSource::AudioSource() {
}

AudioSource::~AudioSource() {
}

void AudioSource::Play() {
    m_Playing = true;
    m_PlaybackSample = 0;
}

void AudioSource::Pause() {
    m_Playing = false;
}

void AudioSource::Stop() {
    m_Playing = false;
    m_PlaybackSample = 0;
}

void AudioSource::SetPlaybackPosition(float seconds) {
    if (m_Clip) {
        m_PlaybackSample = static_cast<size_t>(seconds * m_Clip->GetSampleRate());
    }
}

float AudioSource::GetPlaybackPosition() const {
    if (m_Clip && m_Clip->GetSampleRate() > 0) {
        return static_cast<float>(m_PlaybackSample) / m_Clip->GetSampleRate();
    }
    return 0.0f;
}

AudioEngine::AudioEngine() {
}

AudioEngine::~AudioEngine() {
}

#ifdef __APPLE__

OSStatus AudioEngine::AudioCallback(void* inRefCon,
                                   AudioUnitRenderActionFlags* ioActionFlags,
                                   const AudioTimeStamp* inTimeStamp,
                                   UInt32 inBusNumber,
                                   UInt32 inNumberFrames,
                                   AudioBufferList* ioData) {
    AudioEngine* engine = static_cast<AudioEngine*>(inRefCon);
    
    float* outputL = static_cast<float*>(ioData->mBuffers[0].mData);
    float* outputR = static_cast<float*>(ioData->mBuffers[1].mData);
    
    for (UInt32 i = 0; i < inNumberFrames; ++i) {
        outputL[i] = 0.0f;
        outputR[i] = 0.0f;
    }
    
    engine->MixAudio(outputL, inNumberFrames);
    
    if (ioData->mNumberBuffers > 1) {
        std::memcpy(outputR, outputL, inNumberFrames * sizeof(float));
    }
    
    return noErr;
}

#endif

bool AudioEngine::Initialize() {
#ifdef __APPLE__
    OSStatus status;
    
    status = NewAUGraph(&m_AudioGraph);
    if (status != noErr) {
        std::cerr << "Failed to create audio graph" << std::endl;
        return false;
    }
    
    status = AUGraphOpen(m_AudioGraph);
    if (status != noErr) {
        std::cerr << "Failed to open audio graph" << std::endl;
        return false;
    }
    
    AudioComponentDescription outputDesc;
    outputDesc.componentType = kAudioUnitType_Output;
    outputDesc.componentSubType = kAudioUnitSubType_DefaultOutput;
    outputDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
    outputDesc.componentFlags = 0;
    outputDesc.componentFlagsMask = 0;
    
    AUNode outputNode;
    status = AUGraphAddNode(m_AudioGraph, &outputDesc, &outputNode);
    if (status != noErr) {
        std::cerr << "Failed to add output node" << std::endl;
        return false;
    }
    
    status = AUGraphNodeInfo(m_AudioGraph, outputNode, nullptr, &m_AudioUnit);
    if (status != noErr) {
        std::cerr << "Failed to get audio unit" << std::endl;
        return false;
    }
    
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = AudioCallback;
    callbackStruct.inputProcRefCon = this;
    
    status = AudioUnitSetProperty(m_AudioUnit,
                                  kAudioUnitProperty_SetRenderCallback,
                                  kAudioUnitScope_Input,
                                  0,
                                  &callbackStruct,
                                  sizeof(callbackStruct));
    if (status != noErr) {
        std::cerr << "Failed to set render callback" << std::endl;
        return false;
    }
    
    AudioStreamBasicDescription streamFormat;
    streamFormat.mSampleRate = m_SampleRate;
    streamFormat.mFormatID = kAudioFormatLinearPCM;
    streamFormat.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    streamFormat.mBytesPerPacket = sizeof(float);
    streamFormat.mFramesPerPacket = 1;
    streamFormat.mBytesPerFrame = sizeof(float);
    streamFormat.mChannelsPerFrame = 2;
    streamFormat.mBitsPerChannel = 32;
    
    status = AudioUnitSetProperty(m_AudioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  0,
                                  &streamFormat,
                                  sizeof(streamFormat));
    if (status != noErr) {
        std::cerr << "Failed to set stream format" << std::endl;
        return false;
    }
    
    status = AUGraphInitialize(m_AudioGraph);
    if (status != noErr) {
        std::cerr << "Failed to initialize audio graph" << std::endl;
        return false;
    }
    
    status = AUGraphStart(m_AudioGraph);
    if (status != noErr) {
        std::cerr << "Failed to start audio graph" << std::endl;
        return false;
    }
    
    std::cout << "Audio engine initialized with sample rate: " << m_SampleRate << std::endl;
    return true;
#else
    std::cout << "Audio engine requires macOS" << std::endl;
    return false;
#endif
}

void AudioEngine::Shutdown() {
#ifdef __APPLE__
    if (m_AudioGraph) {
        AUGraphStop(m_AudioGraph);
        AUGraphUninitialize(m_AudioGraph);
        AUGraphClose(m_AudioGraph);
        DisposeAUGraph(m_AudioGraph);
        m_AudioGraph = nullptr;
    }
#endif
    
    m_Sources.clear();
    m_Clips.clear();
    
    std::cout << "Audio engine shut down" << std::endl;
}

void AudioEngine::Update() {
}

std::shared_ptr<AudioClip> AudioEngine::LoadClip(const std::string& path) {
    auto clip = std::make_shared<AudioClip>();
    if (clip->LoadFromFile(path)) {
        m_Clips[clip->GetUUID()] = clip;
        return clip;
    }
    return nullptr;
}

std::shared_ptr<AudioSource> AudioEngine::CreateSource() {
    auto source = std::make_shared<AudioSource>();
    m_Sources.push_back(source);
    return source;
}

void AudioEngine::MixAudio(float* outputBuffer, size_t frameCount) {
    for (auto& source : m_Sources) {
        if (!source->IsPlaying() || !source->GetClip()) continue;
        
        auto clip = source->GetClip();
        const float* clipData = clip->GetData();
        size_t clipSize = clip->GetDataSize();
        
        float volume = source->GetVolume() * m_MasterVolume;
        float attenuation = 1.0f;
        
        if (source->IsSpatial()) {
            attenuation = CalculateAttenuation(source.get());
        }
        
        volume *= attenuation;
        
        for (size_t i = 0; i < frameCount; ++i) {
            if (source->m_PlaybackSample >= clipSize) {
                if (source->IsLooping()) {
                    source->m_PlaybackSample = 0;
                } else {
                    source->Stop();
                    break;
                }
            }
            
            float sample = clipData[source->m_PlaybackSample] * volume;
            outputBuffer[i] += sample;
            
            source->m_PlaybackSample++;
        }
    }
}

float AudioEngine::CalculateAttenuation(const AudioSource* source) const {
    Math::Vector3 listenerPos = m_Listener.GetPosition();
    Math::Vector3 sourcePos = source->GetPosition();
    
    Math::Vector3 diff = sourcePos - listenerPos;
    float distance = diff.Length();
    
    float minDist = source->GetMinDistance();
    float maxDist = source->GetMaxDistance();
    
    if (distance < minDist) {
        return 1.0f;
    }
    
    if (distance > maxDist) {
        return 0.0f;
    }
    
    float attenuation = 1.0f - ((distance - minDist) / (maxDist - minDist));
    return attenuation;
}

void AudioEngine::ApplySpatialAudio(float* samples, size_t sampleCount, const AudioSource* source) {
}

}
