#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include "../Utils/UUID.hpp"

namespace Orchard {

class Resource {
public:
    virtual ~Resource() = default;
    
    const UUID& GetUUID() const { return m_UUID; }
    const std::string& GetPath() const { return m_Path; }
    
    bool IsLoaded() const { return m_Loaded; }
    void SetLoaded(bool loaded) { m_Loaded = loaded; }
    
protected:
    UUID m_UUID;
    std::string m_Path;
    bool m_Loaded = false;
};

class ResourceManager {
public:
    bool Initialize();
    void Shutdown();
    
    template<typename T>
    std::shared_ptr<T> Load(const std::string& path);
    
    template<typename T>
    std::shared_ptr<T> Get(const UUID& uuid);
    
    template<typename T>
    void Unload(const UUID& uuid);
    
    void UnloadAll();
    
    template<typename T>
    void RegisterLoader(std::function<std::shared_ptr<T>(const std::string&)> loader);
    
private:
    struct ResourceEntry {
        std::shared_ptr<Resource> resource;
        std::type_index type;
        
        ResourceEntry(std::shared_ptr<Resource> res, std::type_index t)
            : resource(std::move(res)), type(t) {}
    };
    
    std::unordered_map<UUID, ResourceEntry> m_Resources;
    std::unordered_map<std::string, UUID> m_PathToUUID;
    
    template<typename T>
    using LoaderFunc = std::function<std::shared_ptr<T>(const std::string&)>;
    
    std::unordered_map<std::type_index, void*> m_Loaders;
};

template<typename T>
std::shared_ptr<T> ResourceManager::Load(const std::string& path) {
    auto it = m_PathToUUID.find(path);
    if (it != m_PathToUUID.end()) {
        return Get<T>(it->second);
    }
    
    auto loaderIt = m_Loaders.find(std::type_index(typeid(T)));
    if (loaderIt == m_Loaders.end()) {
        return nullptr;
    }
    
    auto loader = static_cast<LoaderFunc<T>*>(loaderIt->second);
    auto resource = (*loader)(path);
    
    if (resource) {
        m_Resources.emplace(resource->GetUUID(), 
            ResourceEntry(resource, std::type_index(typeid(T))));
        m_PathToUUID[path] = resource->GetUUID();
    }
    
    return resource;
}

template<typename T>
std::shared_ptr<T> ResourceManager::Get(const UUID& uuid) {
    auto it = m_Resources.find(uuid);
    if (it != m_Resources.end() && it->second.type == std::type_index(typeid(T))) {
        return std::static_pointer_cast<T>(it->second.resource);
    }
    return nullptr;
}

template<typename T>
void ResourceManager::Unload(const UUID& uuid) {
    m_Resources.erase(uuid);
}

template<typename T>
void ResourceManager::RegisterLoader(std::function<std::shared_ptr<T>(const std::string&)> loader) {
    m_Loaders[std::type_index(typeid(T))] = new LoaderFunc<T>(std::move(loader));
}

}
