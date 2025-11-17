#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Orchard {

class Scene;
class Renderer;

class SceneManager {
public:
    bool Initialize();
    void Shutdown();
    
    void Update(double deltaTime);
    void Render(Renderer* renderer);
    
    std::shared_ptr<Scene> CreateScene(const std::string& name);
    bool LoadScene(const std::string& path);
    bool SaveScene(const std::string& path);
    
    void SetActiveScene(std::shared_ptr<Scene> scene);
    std::shared_ptr<Scene> GetActiveScene() const { return m_ActiveScene; }
    
private:
    std::shared_ptr<Scene> m_ActiveScene;
    std::vector<std::shared_ptr<Scene>> m_Scenes;
};

}
