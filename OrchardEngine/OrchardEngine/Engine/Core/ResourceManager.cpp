#include "ResourceManager.hpp"
#include <iostream>

namespace Orchard {

bool ResourceManager::Initialize() {
    std::cout << "Resource Manager initialized" << std::endl;
    return true;
}

void ResourceManager::Shutdown() {
    UnloadAll();
    
    for (auto& [typeIndex, loader] : m_Loaders) {
        delete loader;
    }
    m_Loaders.clear();
    
    std::cout << "Resource Manager shut down" << std::endl;
}

void ResourceManager::UnloadAll() {
    m_Resources.clear();
    m_PathToUUID.clear();
}

}
