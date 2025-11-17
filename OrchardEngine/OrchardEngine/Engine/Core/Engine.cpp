#include "Engine.hpp"
#include "../Rendering/Renderer.hpp"
#include "../Physics/PhysicsWorld.hpp"
#include "../Audio/AudioEngine.hpp"
#include "ResourceManager.hpp"
#include "SceneManager.hpp"
#include "EventSystem.hpp"
#include <chrono>
#include <thread>
#include <iostream>

namespace Orchard {

Engine& Engine::Instance() {
    static Engine instance;
    return instance;
}

bool Engine::Initialize(const std::string& appName, uint32_t width, uint32_t height) {
    if (m_Initialized) {
        std::cerr << "Engine already initialized!" << std::endl;
        return false;
    }
    
    std::cout << "Initializing Orchard Engine..." << std::endl;
    
    m_EventSystem = std::make_unique<EventSystem>();
    
    m_ResourceManager = std::make_unique<ResourceManager>();
    if (!m_ResourceManager->Initialize()) {
        std::cerr << "Failed to initialize Resource Manager" << std::endl;
        return false;
    }
    
    m_Renderer = std::make_unique<Renderer>();
    if (!m_Renderer->Initialize(appName, width, height)) {
        std::cerr << "Failed to initialize Renderer" << std::endl;
        return false;
    }
    
    m_PhysicsWorld = std::make_unique<PhysicsWorld>();
    if (!m_PhysicsWorld->Initialize()) {
        std::cerr << "Failed to initialize Physics World" << std::endl;
        return false;
    }
    
    m_AudioEngine = std::make_unique<AudioEngine>();
    if (!m_AudioEngine->Initialize()) {
        std::cerr << "Failed to initialize Audio Engine" << std::endl;
        return false;
    }
    
    m_SceneManager = std::make_unique<SceneManager>();
    if (!m_SceneManager->Initialize()) {
        std::cerr << "Failed to initialize Scene Manager" << std::endl;
        return false;
    }
    
    m_Initialized = true;
    std::cout << "Orchard Engine initialized successfully!" << std::endl;
    return true;
}

void Engine::Shutdown() {
    if (!m_Initialized) return;
    
    std::cout << "Shutting down Orchard Engine..." << std::endl;
    
    m_SceneManager->Shutdown();
    m_AudioEngine->Shutdown();
    m_PhysicsWorld->Shutdown();
    m_Renderer->Shutdown();
    m_ResourceManager->Shutdown();
    
    m_SceneManager.reset();
    m_AudioEngine.reset();
    m_PhysicsWorld.reset();
    m_Renderer.reset();
    m_ResourceManager.reset();
    m_EventSystem.reset();
    
    m_Initialized = false;
    std::cout << "Orchard Engine shut down successfully." << std::endl;
}

void Engine::Run() {
    if (!m_Initialized) {
        std::cerr << "Cannot run engine before initialization!" << std::endl;
        return;
    }
    
    m_Running = true;
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;
    const double fixedTimeStep = 1.0 / 60.0;
    
    while (m_Running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;
        lastTime = currentTime;
        
        m_DeltaTime = elapsed.count();
        m_TotalTime += m_DeltaTime;
        
        accumulator += m_DeltaTime;
        while (accumulator >= fixedTimeStep) {
            FixedUpdate(fixedTimeStep);
            accumulator -= fixedTimeStep;
        }
        
        Update(m_DeltaTime);
        Render();
        
        m_FrameCount++;
        
        if (m_TargetFPS > 0) {
            double targetFrameTime = 1.0 / m_TargetFPS;
            auto frameEndTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> frameTime = frameEndTime - currentTime;
            
            if (frameTime.count() < targetFrameTime) {
                std::this_thread::sleep_for(
                    std::chrono::duration<double>(targetFrameTime - frameTime.count())
                );
            }
        }
    }
}

void Engine::RequestExit() {
    m_Running = false;
}

void Engine::SetTargetFrameRate(uint32_t fps) {
    m_TargetFPS = fps;
}

void Engine::Update(double deltaTime) {
    m_SceneManager->Update(deltaTime);
}

void Engine::FixedUpdate(double fixedDeltaTime) {
    m_PhysicsWorld->Step(fixedDeltaTime);
}

void Engine::Render() {
    m_Renderer->BeginFrame();
    m_SceneManager->Render(m_Renderer.get());
    m_Renderer->EndFrame();
}

}
