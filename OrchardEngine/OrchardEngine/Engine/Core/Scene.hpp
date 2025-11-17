#pragma once

#include <string>
#include <memory>
#include "../ECS/World.hpp"

namespace Orchard {

class Renderer;

class Scene {
public:
    Scene(const std::string& name);
    ~Scene();
    
    void Update(double deltaTime);
    void Render(Renderer* renderer);
    
    const std::string& GetName() const { return m_Name; }
    
    ECS::World* GetWorld() { return m_World.get(); }
    
private:
    std::string m_Name;
    std::unique_ptr<ECS::World> m_World;
};

}
