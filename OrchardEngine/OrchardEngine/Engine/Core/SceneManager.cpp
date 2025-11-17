#include "SceneManager.hpp"
#include "Scene.hpp"
#include <iostream>

namespace Orchard {

bool SceneManager::Initialize() {
    std::cout << "Scene Manager initialized" << std::endl;
    return true;
}

void SceneManager::Shutdown() {
    m_ActiveScene.reset();
    m_Scenes.clear();
    std::cout << "Scene Manager shut down" << std::endl;
}

void SceneManager::Update(double deltaTime) {
    if (m_ActiveScene) {
        m_ActiveScene->Update(deltaTime);
    }
}

void SceneManager::Render(Renderer* renderer) {
    if (m_ActiveScene) {
        m_ActiveScene->Render(renderer);
    }
}

std::shared_ptr<Scene> SceneManager::CreateScene(const std::string& name) {
    auto scene = std::make_shared<Scene>(name);
    m_Scenes.push_back(scene);
    
    if (!m_ActiveScene) {
        m_ActiveScene = scene;
    }
    
    return scene;
}

bool SceneManager::LoadScene(const std::string& path) {
    std::cout << "Loading scene from: " << path << std::endl;
    return false;
}

bool SceneManager::SaveScene(const std::string& path) {
    std::cout << "Saving scene to: " << path << std::endl;
    return false;
}

void SceneManager::SetActiveScene(std::shared_ptr<Scene> scene) {
    m_ActiveScene = scene;
}

}
