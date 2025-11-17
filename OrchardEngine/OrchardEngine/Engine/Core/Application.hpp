#pragma once

#include <string>

namespace Orchard {

class Application {
public:
    Application(const std::string& name);
    virtual ~Application();
    
    virtual bool OnInitialize() { return true; }
    virtual void OnShutdown() {}
    
    virtual void OnUpdate(double deltaTime) {}
    virtual void OnFixedUpdate(double fixedDeltaTime) {}
    virtual void OnRender() {}
    
    void Run();
    void Exit();
    
    const std::string& GetName() const { return m_Name; }
    
protected:
    std::string m_Name;
    bool m_ShouldRun = true;
};

}
