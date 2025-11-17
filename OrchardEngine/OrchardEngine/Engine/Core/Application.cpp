#include "Application.hpp"
#include "Engine.hpp"

namespace Orchard {

Application::Application(const std::string& name) : m_Name(name) {
}

Application::~Application() {
}

void Application::Run() {
    Engine& engine = Engine::Instance();
    
    if (!engine.Initialize(m_Name, 1280, 720)) {
        return;
    }
    
    if (!OnInitialize()) {
        engine.Shutdown();
        return;
    }
    
    engine.Run();
    
    OnShutdown();
    engine.Shutdown();
}

void Application::Exit() {
    Engine::Instance().RequestExit();
}

}
