#pragma once

#include "../Math/Vector.hpp"
#include "../Utils/UUID.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#ifdef __APPLE__
#include <AudioToolbox/AudioToolbox.h>
#include <AVFoundation/AVFoundation.h>
#endif

namespace Orchard::Audio {

enum class AudioFormat {
    Mono8,
    Mono16,
    Stereo8,
    Stereo16,
    Float32
};

class AudioClip {
public:
    AudioClip();
    ~AudioClip();
    
    bool LoadFromFile(const std::string& path);
    bool LoadFromMemory(const void* data, size_t size, AudioFormat format, uint32_t sampleRate);
    
    const UUID& GetUUID() const { return m_UUID; }
    
    uint32_t GetSampleRate() const { return m_SampleRate; }
    uint32_t GetChannels() const { return m_Channels; }
    uint32_t GetSampleCount() const { return m_SampleCount; }
    
    const float* GetData() const { return m_Data.data(); }
    size_t GetDataSize() const { return m_Data.size(); }
    
private:
    UUID m_UUID;
    std::vector<float> m_Data;
    uint32_t m_SampleRate = 44100;
    uint32_t m_Channels = 2;
    uint32_t m_SampleCount = 0;
};

class AudioSource {
public:
    AudioSource();
    ~AudioSource();
    
    void SetClip(std::shared_ptr<AudioClip> clip) { m_Clip = clip; }
    std::shared_ptr<AudioClip> GetClip() const { return m_Clip; }
    
    void SetPosition(const Math::Vector3& position) { m_Position = position; }
    const Math::Vector3& GetPosition() const { return m_Position; }
    
    void SetVolume(float volume) { m_Volume = volume; }
    float GetVolume() const { return m_Volume; }
    
    void SetPitch(float pitch) { m_Pitch = pitch; }
    float GetPitch() const { return m_Pitch; }
    
    void SetLoop(bool loop) { m_Loop = loop; }
    bool IsLooping() const { return m_Loop; }
    
    void SetSpatial(bool spatial) { m_Spatial = spatial; }
    bool IsSpatial() const { return m_Spatial; }
    
    void SetMinDistance(float distance) { m_MinDistance = distance; }
    float GetMinDistance() const { return m_MinDistance; }
    
    void SetMaxDistance(float distance) { m_MaxDistance = distance; }
    float GetMaxDistance() const { return m_MaxDistance; }
    
    void Play();
    void Pause();
    void Stop();
    
    bool IsPlaying() const { return m_Playing; }
    
    void SetPlaybackPosition(float seconds);
    float GetPlaybackPosition() const;
    
private:
    std::shared_ptr<AudioClip> m_Clip;
    
    Math::Vector3 m_Position{0, 0, 0};
    
    float m_Volume = 1.0f;
    float m_Pitch = 1.0f;
    bool m_Loop = false;
    bool m_Spatial = false;
    float m_MinDistance = 1.0f;
    float m_MaxDistance = 500.0f;
    
    bool m_Playing = false;
    size_t m_PlaybackSample = 0;
};

class AudioListener {
public:
    void SetPosition(const Math::Vector3& position) { m_Position = position; }
    const Math::Vector3& GetPosition() const { return m_Position; }
    
    void SetOrientation(const Math::Vector3& forward, const Math::Vector3& up) {
        m_Forward = forward;
        m_Up = up;
    }
    
    const Math::Vector3& GetForward() const { return m_Forward; }
    const Math::Vector3& GetUp() const { return m_Up; }
    
private:
    Math::Vector3 m_Position{0, 0, 0};
    Math::Vector3 m_Forward{0, 0, 1};
    Math::Vector3 m_Up{0, 1, 0};
};

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    
    bool Initialize();
    void Shutdown();
    
    void Update();
    
    std::shared_ptr<AudioClip> LoadClip(const std::string& path);
    std::shared_ptr<AudioSource> CreateSource();
    
    void SetMasterVolume(float volume) { m_MasterVolume = volume; }
    float GetMasterVolume() const { return m_MasterVolume; }
    
    AudioListener* GetListener() { return &m_Listener; }
    
    void EnableSpatialAudio(bool enable) { m_SpatialAudioEnabled = enable; }
    bool IsSpatialAudioEnabled() const { return m_SpatialAudioEnabled; }
    
private:
    void MixAudio(float* outputBuffer, size_t frameCount);
    float CalculateAttenuation(const AudioSource* source) const;
    void ApplySpatialAudio(float* samples, size_t sampleCount, const AudioSource* source);
    
#ifdef __APPLE__
    AudioUnit m_AudioUnit;
    AUGraph m_AudioGraph;
    static OSStatus AudioCallback(void* inRefCon,
                                  AudioUnitRenderActionFlags* ioActionFlags,
                                  const AudioTimeStamp* inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList* ioData);
#endif
    
    std::unordered_map<UUID, std::shared_ptr<AudioClip>> m_Clips;
    std::vector<std::shared_ptr<AudioSource>> m_Sources;
    
    AudioListener m_Listener;
    
    float m_MasterVolume = 1.0f;
    bool m_SpatialAudioEnabled = true;
    
    uint32_t m_SampleRate = 48000;
    uint32_t m_BufferSize = 512;
};

}
