#include "Scene.hpp"
#include "../Rendering/Renderer.hpp"

namespace Orchard {

Scene::Scene(const std::string& name) : m_Name(name) {
    m_World = std::make_unique<ECS::World>();
}

Scene::~Scene() {
}

void Scene::Update(double deltaTime) {
    if (m_World) {
        m_World->Update(deltaTime);
    }
}

void Scene::Render(Renderer* renderer) {
}

}
