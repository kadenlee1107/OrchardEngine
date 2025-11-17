#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Orchard {

class Application;
class Renderer;
class PhysicsWorld;
class AudioEngine;
class ResourceManager;
class SceneManager;
class EventSystem;

class Engine {
public:
    static Engine& Instance();
    
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    
    bool Initialize(const std::string& appName, uint32_t width, uint32_t height);
    void Shutdown();
    
    void Run();
    void RequestExit();
    
    Renderer* GetRenderer() const { return m_Renderer.get(); }
    PhysicsWorld* GetPhysicsWorld() const { return m_PhysicsWorld.get(); }
    AudioEngine* GetAudioEngine() const { return m_AudioEngine.get(); }
    ResourceManager* GetResourceManager() const { return m_ResourceManager.get(); }
    SceneManager* GetSceneManager() const { return m_SceneManager.get(); }
    EventSystem* GetEventSystem() const { return m_EventSystem.get(); }
    
    double GetDeltaTime() const { return m_DeltaTime; }
    double GetTotalTime() const { return m_TotalTime; }
    uint64_t GetFrameCount() const { return m_FrameCount; }
    
    void SetTargetFrameRate(uint32_t fps);
    uint32_t GetTargetFrameRate() const { return m_TargetFPS; }
    
private:
    Engine() = default;
    ~Engine() = default;
    
    void Update(double deltaTime);
    void FixedUpdate(double fixedDeltaTime);
    void Render();
    
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<PhysicsWorld> m_PhysicsWorld;
    std::unique_ptr<AudioEngine> m_AudioEngine;
    std::unique_ptr<ResourceManager> m_ResourceManager;
    std::unique_ptr<SceneManager> m_SceneManager;
    std::unique_ptr<EventSystem> m_EventSystem;
    
    double m_DeltaTime = 0.0;
    double m_TotalTime = 0.0;
    uint64_t m_FrameCount = 0;
    uint32_t m_TargetFPS = 60;
    
    bool m_Running = false;
    bool m_Initialized = false;
};

}
