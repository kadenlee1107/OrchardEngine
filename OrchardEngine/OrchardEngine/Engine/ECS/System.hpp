#pragma once

namespace Orchard::ECS {

class World;

class System {
public:
    virtual ~System() = default;
    
    virtual void OnInit(World* world) {}
    virtual void OnUpdate(World* world, double deltaTime) = 0;
    virtual void OnShutdown(World* world) {}
    
protected:
    bool m_Enabled = true;
};

}
