#pragma once

#include "Entity.hpp"
#include "Component.hpp"
#include "Archetype.hpp"
#include "System.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace Orchard::ECS {

class World {
public:
    World();
    ~World();
    
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    bool IsEntityValid(Entity entity) const;
    
    template<typename T>
    void AddComponent(Entity entity, const T& component);
    
    template<typename T>
    void RemoveComponent(Entity entity);
    
    template<typename T>
    T* GetComponent(Entity entity);
    
    template<typename T>
    bool HasComponent(Entity entity) const;
    
    template<typename... Components>
    void ForEach(std::function<void(Entity, Components&...)> callback);
    
    void AddSystem(std::unique_ptr<System> system);
    
    void Update(double deltaTime);
    
private:
    struct EntityRecord {
        Archetype* archetype = nullptr;
        size_t indexInArchetype = 0;
        bool alive = false;
    };
    
    std::vector<EntityRecord> m_EntityRecords;
    std::vector<EntityID> m_FreeEntities;
    EntityID m_NextEntityID = 1;
    
    std::unordered_map<uint64_t, std::unique_ptr<Archetype>> m_Archetypes;
    std::vector<std::unique_ptr<System>> m_Systems;
    
    Archetype* GetOrCreateArchetype(const std::vector<ComponentTypeID>& componentTypes);
    uint64_t GetArchetypeHash(const std::vector<ComponentTypeID>& componentTypes);
    
    void MoveEntity(Entity entity, Archetype* newArchetype);
};

template<typename T>
void World::AddComponent(Entity entity, const T& component) {
    if (!IsEntityValid(entity)) return;
    
    EntityRecord& record = m_EntityRecords[entity.id];
    
    std::vector<ComponentTypeID> newTypes;
    if (record.archetype) {
        for (const auto& info : record.archetype->GetComponentTypes()) {
            newTypes.push_back(info.typeID);
        }
    }
    newTypes.push_back(ComponentRegistry::GetTypeID<T>());
    
    Archetype* newArchetype = GetOrCreateArchetype(newTypes);
    MoveEntity(entity, newArchetype);
    
    T* componentPtr = record.archetype->GetComponent<T>(record.indexInArchetype);
    if (componentPtr) {
        *componentPtr = component;
    }
}

template<typename T>
T* World::GetComponent(Entity entity) {
    if (!IsEntityValid(entity)) return nullptr;
    
    EntityRecord& record = m_EntityRecords[entity.id];
    if (!record.archetype) return nullptr;
    
    return record.archetype->GetComponent<T>(record.indexInArchetype);
}

template<typename T>
bool World::HasComponent(Entity entity) const {
    if (!IsEntityValid(entity)) return false;
    
    const EntityRecord& record = m_EntityRecords[entity.id];
    if (!record.archetype) return false;
    
    ComponentTypeID typeID = ComponentRegistry::GetTypeID<T>();
    for (const auto& info : record.archetype->GetComponentTypes()) {
        if (info.typeID == typeID) return true;
    }
    return false;
}

}
